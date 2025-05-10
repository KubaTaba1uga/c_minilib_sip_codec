#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "parser/iterator/line_iterator.h"
#include "parser/parser.h"
#include "sipmsg/sipmsg.h"
#include "utils/dynamic_buffer.h"

#ifndef CMSC_PARSER_DYNBUF_SIZE
#define CMSC_PARSER_DYNBUF_SIZE 2048
#endif

cme_error_t cmsc_parser_create(cmsc_parser_t *parser) {
  cmsc_parser_t local_parser;
  cme_error_t err;

  if (!(local_parser = malloc(sizeof(struct cmsc_Parser) +
                              sizeof(char) * CMSC_PARSER_DYNBUF_SIZE))) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `local_parser`");
    goto error_out;
  }

  if ((err =
           cmsc_dynbuf_init(CMSC_PARSER_DYNBUF_SIZE, &local_parser->content))) {
    goto error_parser_cleanup;
  }

  local_parser->msg = NULL;
  local_parser->state = cmsc_ParserStates_MsgEmpty;

  *parser = local_parser;

  return 0;

error_parser_cleanup:
  free(local_parser);
error_out:
  return cme_return(err);
}

void cmsc_parser_destroy(cmsc_parser_t *parser) {
  if (!parser || !*parser) {
    return;
  }

  free((*parser)->msg);
  free(*parser);

  *parser = NULL;
};

enum cmsc_ParserStates cmsc_parser_get_state(cmsc_parser_t parser) {
  if (!parser) {
    return cmsc_ParserStates_MsgEmpty;
  }

  return parser->state;
};

cme_error_t cmsc_parser_feed_data(struct cmsc_CharBufferView data,
                                  cmsc_parser_t *parser) {
  cme_error_t err;
  if (!parser || !*parser) {
    err = cme_error(ENOMEM, "`parser` cannot be NULL");
    goto error_out;
  }

  // We need to store whole data in case some data from one feed
  //   will be needed in next feed.
  if ((err =
           cmsc_dynbuf_put(data.buf_len, (char *)data.buf, (void **)parser,
                           sizeof(struct cmsc_Parser), &(*parser)->content))) {
    goto error_out;
  };

  struct cmsc_HeaderIterator header_iter;
  if ((err = cmsc_headeriter_init(&header_iter))) {
    goto error_out;
  }

  bool is_next;
  do {
    is_next = false;

    switch ((*parser)->state) {
    case cmsc_ParserStates_MsgEmpty:
      err = cmsc_parser_parse_msgempty(&header_iter, *parser, &is_next);
      break;
    case cmsc_ParserStates_ParsingHeaders:
      err = cmsc_parser_parse_headers(&header_iter, *parser, &is_next);
      break;
    case cmsc_ParserStates_ParsingBody:
      // ParsingHeaders -> ParsingBody
    case cmsc_ParserStates_MsgReady:
      // This is final state, to go back to MsgEmpty
      //  one needs to pop_msg.
      break;
    default:;
    }

    if (err) {
      goto error_out;
    }

    printf("is_next=%d, (*parser)->state=%d\n", is_next, (*parser)->state);
    if (is_next) {
      (*parser)->state = ((*parser)->state % cmsc_ParserStates_MsgReady) + 1;
    }
  } while (is_next);

  // Once we processed all data we can flush the parser content.
  //  There may be some dataloss here that needs to be fixed someday.
  cmsc_dynbuf_flush(&(*parser)->content);

  return 0;

error_out:
  return cme_return(err);
};
