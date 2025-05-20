# c_minilib_sip_codec

**`c_minilib_sip_codec`** is a lightweight, modular C library for **parsing, decoding, and generating SIP messages**. Designed for embedded systems and minimalist runtimes, it offers structured access to SIP fields, extensible encoding/decoding logic, and clean memory handling.

---

## ✨ Features

* **Full SIP Message Lifecycle**: Supports parsing and generating SIP requests/responses.
* **Structured Field Access**: Direct access to decoded fields like `From`, `To`, `CSeq`, `Via`, `Call-ID`, etc.
* **Custom Header Support**: Arbitrary headers preserved and handled generically.
* **Macro-Free C API**: Explicit, predictable interface—ideal for embedded or static analysis-sensitive environments.
* **Modular Design**: Header-based decoder/encoder dispatch for easy extension.
* **Comprehensive Tests**: Full test coverage with [Unity](https://www.throwtheswitch.org/unity).
* **Safe Buffer Management**: Internal dynamic buffer management to avoid overflows.

---

## 📥 Example Usage

### generate.c

```c
#include <c_minilib_sip_codec.h>
#include <stdio.h>

int main(void) {
  struct cmsc_SipMessage *msg = NULL;
  const char *buf = NULL;
  uint32_t len = 0;

  cmsc_init();
  cmsc_sipmsg_create_with_buf(&msg);

  cmsc_sipmsg_insert_request_line(
      strlen("SIP/2.0"), "SIP/2.0",
      strlen("sip:bob@example.com"), "sip:bob@example.com",
      strlen("INVITE"), "INVITE", msg);

  cmsc_sipmsg_insert_from(strlen("<sip:alice@example.com>"),
                          "<sip:alice@example.com>", strlen("123"), "123", msg);

  cmsc_sipmsg_insert_to(strlen("<sip:bob@example.com>"),
                        "<sip:bob@example.com>", strlen("456"), "456", msg);

  cmsc_generate_sip(msg, &len, &buf);
  fwrite(buf, 1, len, stdout);

  free((void *)buf);
  cmsc_sipmsg_destroy_with_buf(&msg);
  cmsc_destroy();
  return 0;
}
```

### parse.c

```c
#include <c_minilib_sip_codec.h>
#include <stdio.h>

int main(void) {
  cmsc_init();
  struct cmsc_SipMessage *msg = NULL;

  const char *raw_msg = "INVITE sip:bob@example.com SIP/2.0\r\n"
                        "From: <sip:alice@example.com>;tag=123\r\n"
                        "To: <sip:bob@example.com>;tag=456\r\n"
                        "Call-ID: abc123\r\n"
                        "CSeq: 1 INVITE\r\n\r\n";

  cmsc_parse_sip(strlen(raw_msg), raw_msg, &msg);

  struct cmsc_String from = cmsc_bs_msg_to_string(&msg->from.uri, msg);
  printf("From URI: %.*s\n", from.len, from.buf);

  cmsc_sipmsg_destroy(&msg);
  cmsc_destroy();
  return 0;
}
```

---

## ⚙️ Build Instructions

Using [Meson](https://mesonbuild.com/):

```sh
meson setup build
meson compile -C build
```

---

## ✅ Run Tests

```sh
meson test -C build
```

Includes test coverage for:

* Parsing requests and responses
* Header decoding (To, From, Via, etc.)
* Buffer overflows
* SIP generation and reconstruction

---

## 🛠️ Development Tools

Automated using [Invoke](https://www.pyinvoke.org/):

```sh
inv install     # Install dependencies
inv build       # Build with Meson
inv test        # Run all tests
inv lint        # Run clang-tidy checks
inv format      # Apply clang-format
inv clean       # Clean build artifacts
```

Supports `--debug` and `--backtrace` build flags.

---

## 🧠 SIP Decoding Logic

Decoding is modular:

* Recognized headers are parsed and attached directly to `cmsc_SipMessage`.
* Remaining headers are preserved as generic `key/value` pairs.
* Structured access through buffer-backed string views.
* Includes iterator-based parsing for arguments like `branch`, `ttl`, `received`.

---

## 🖋️ Example Outputs

Example-generated and parsed messages are available in the `example/` directory:
- `generate.c`: Creates a SIP request and writes it to a file named message.sip.
- `parse.c`: Loads the message.sip file, parses it using cmsc_parse_sip(), and prints key fields such as From, To, Call-ID, CSeq, and message body to stdout.

---

## 📄 License

MIT License. See [LICENSE](LICENSE) for full details.
