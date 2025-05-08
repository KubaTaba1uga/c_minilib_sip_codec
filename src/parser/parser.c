#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "utils/dynamic_buffer.h"

#define CMSC_PARSER_DYNBUF_SIZE 2048

struct cmsc_Parser {
  enum cmsc_ParserStates state;
  cmsc_sipmsg_t msg;
  struct cmsc_DynamicBuffer *content;
};

/* cme_error_t cmsc_parser_create(cmsc_parser_t *parser) { */
/*   cmsc_parser_t local_parse; */
/*   cme_error_t err; */

/*   if (!(local_parse = malloc(sizeof(struct cmsc_Parser)))) { */
/*     err = cme_error(ENOMEM, "Cannot allocate memory for `local_parse`"); */
/*     goto error_out; */
/*   } */

/*   local_parse->msg; */

/*   local_parse->state = cmsc_ParserStates_MsgEmpty; */

/*   if ((err = cmsc_dynbuf_create(CMSC_PARSER_DYNBUF_SIZE, */
/*                                 &local_parse->content))) { */
/*     goto error_parser_cleanup; */
/*   }; */

/*   return 0; */

/* error_parser_cleanup: */
/*   free(local_parse); */
/* error_out: */
/*   return cme_return(err); */
/* } */
