/*
 * First KLEE tutorial: testing a small function
 * http://klee.github.io/tutorials/testing-function/
 */
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#define FLB_FALSE  0
#define FLB_TRUE   !FLB_FALSE
#define FLB_UTF8_ACCEPT 0
#define FLB_UTILS_FRAGMENT_PRIVATE_BLOCK_DESCRIPTOR 0xE0
#define FLB_UTF8_ACCEPT 0
#define FLB_UTF8_REJECT 1

const uint8_t utf8d[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 00..1f
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 20..3f
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 40..5f
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 60..7f
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 80..9f
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, // a0..bf
    8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // c0..df
    0xa,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x3,0x3, // e0..ef
    0xb,0x6,0x6,0x6,0x5,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // f0..ff
    0x0,0x1,0x2,0x3,0x5,0x8,0x7,0x1,0x1,0x1,0x4,0x6,0x1,0x1,0x1,0x1, // s0..s0
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1, // s1..s2
    1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1, // s3..s4
    1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,3,1,1,1,1,1,1, // s5..s6
    1,3,1,1,1,1,1,3,1,3,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // s7..s8
};

uint32_t flb_utf8_decode(uint32_t *state, uint32_t *codep,
                                       uint32_t byte)
{
    uint32_t type = utf8d[byte];

    *codep = (*state != FLB_UTF8_ACCEPT) ?
        (byte & 0x3fu) | (*codep << 6) :
        (0xff >> type) & (byte);

    *state = utf8d[256 + *state*16 + type];
    return *state;
}


const char trailingBytesForUTF8[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

/* returns length of next utf-8 sequence */
int flb_utf8_len(const char *s)
{
    return trailingBytesForUTF8[(unsigned int)(unsigned char)s[0]] + 1;
}

void encoded_to_buf(char *out, const char *in, int len)
{
    int i;
    char *p = out;

    for (i = 0; i < len; i++) {
        *p++ = in[i];
    }
}
void flb_warn(char *inNull)
{}
void flb_debug(char *inNull)
{}
int flb_utils_write_str(char *buf, int *off, size_t size,
                        const char *str, size_t str_len)
{
    int i;
    int b;
    int ret;
    int written = 0;
    int required;
    int len;
    int hex_bytes;
    int is_valid;
    int utf_sequence_number;
    int utf_sequence_length;
    uint32_t codepoint;
    uint32_t state = 0;
    char tmp[16];
    size_t available;
    uint32_t c;
    char *p;
    uint8_t *s;

    available = (size - *off);
    required = str_len;
    if (available <= required) {
        return FLB_FALSE;
    }

    p = buf + *off;
    for (i = 0; i < str_len; i++) {
        if ((available - written) < 2) {
            return FLB_FALSE;
        }

        c = (uint32_t) str[i];
        if (c == '\"') {
            *p++ = '\\';
            *p++ = '\"';
        }
        else if (c == '\\') {
            *p++ = '\\';
            *p++ = '\\';
        }
        else if (c == '\n') {
            *p++ = '\\';
            *p++ = 'n';
        }
        else if (c == '\r') {
            *p++ = '\\';
            *p++ = 'r';
        }
        else if (c == '\t') {
            *p++ = '\\';
            *p++ = 't';
        }
        else if (c == '\b') {
            *p++ = '\\';
            *p++ = 'b';
        }
        else if (c == '\f') {
            *p++ = '\\';
            *p++ = 'f';
        }
        else if (c < 32 || c == 0x7f) {
            if ((available - written) < 6) {
                return FLB_FALSE;
            }
            len = snprintf(tmp, sizeof(tmp) - 1, "\\u%.4hhx", (unsigned char) c);
            if ((available - written) < len) {
                return FLB_FALSE;
            }
            encoded_to_buf(p, tmp, len);
            p += len;
        }
        else if (c >= 0x80 && c <= 0xFFFF) {
            hex_bytes = flb_utf8_len(str + i);
            if (available - written < 6) {
                return FLB_FALSE;
            }

            if (i + hex_bytes > str_len) {
                break; /* skip truncated UTF-8 */
            }

            state = FLB_UTF8_ACCEPT;
            codepoint = 0;

            for (b = 0; b < hex_bytes; b++) {
                s = (unsigned char *) str + i + b;
                ret = flb_utf8_decode(&state, &codepoint, *s);
                if (ret == 0) {
                    break;
                }
            }

            if (state != FLB_UTF8_ACCEPT) {
                /* Invalid UTF-8 hex, just skip utf-8 bytes */
                flb_warn("[pack] invalid UTF-8 bytes found, skipping bytes");
            }
            else {
                len = snprintf(tmp, sizeof(tmp) - 1, "\\u%.4x", codepoint);
                if ((available - written) < len) {
                    return FLB_FALSE;
                }
                encoded_to_buf(p, tmp, len);
                p += len;
            }
            i += (hex_bytes - 1);
        }
        else if (c > 0xFFFF) {
            utf_sequence_length = flb_utf8_len(str + i);

            if (i + utf_sequence_length > str_len) {
                break; /* skip truncated UTF-8 */
            }

            is_valid = FLB_TRUE;
            for (utf_sequence_number = 0; utf_sequence_number < utf_sequence_length;
                utf_sequence_number++) {
                /* Leading characters must start with bits 11 */
                if (utf_sequence_number == 0 && ((str[i] & 0xC0) != 0xC0)) {
                    /* Invalid unicode character. replace */
                    flb_debug("[pack] unexpected UTF-8 leading byte, "
                             "substituting character with replacement character");
                    tmp[utf_sequence_number] = str[i];
                    ++i; /* Consume invalid leading byte */
                    utf_sequence_length = utf_sequence_number + 1;
                    is_valid = FLB_FALSE;
                    break;
                }
                /* Trailing characters must start with bits 10 */
                else if (utf_sequence_number > 0 && ((str[i] & 0xC0) != 0x80)) {
                    /* Invalid unicode character. replace */
                    flb_debug("[pack] unexpected UTF-8 continuation byte, "
                             "substituting character with replacement character");
                    /* This byte, i, is the start of the next unicode character */
                    utf_sequence_length = utf_sequence_number;
                    is_valid = FLB_FALSE;
                    break;
                }

                tmp[utf_sequence_number] = str[i];
                ++i;
            }
            --i;

            if (is_valid) {
                if (available - written < utf_sequence_length) {
                    return FLB_FALSE;
                }

                encoded_to_buf(p, tmp, utf_sequence_length);
                p += utf_sequence_length;
            }
            else {
                if (available - written < utf_sequence_length * 3) {
                    return FLB_FALSE;
                }

                /*
                 * Utf-8 sequence is invalid. Map fragments to private use area
                 * codepoints in range:
                 * 0x<FLB_UTILS_FRAGMENT_PRIVATE_BLOCK_DESCRIPTOR>00 to
                 * 0x<FLB_UTILS_FRAGMENT_PRIVATE_BLOCK_DESCRIPTOR>FF
                 */
                for (b = 0; b < utf_sequence_length; ++b) {
                    /*
                     * Utf-8 private block invalid hex mapping. Format unicode charpoint
                     * in the following format:
                     *
                     *      +--------+--------+--------+
                     *      |1110PPPP|10PPPPHH|10HHHHHH|
                     *      +--------+--------+--------+
                     *
                     * Where:
                     *   P is FLB_UTILS_FRAGMENT_PRIVATE_BLOCK_DESCRIPTOR bits (1 byte)
                     *   H is Utf-8 fragment hex bits (1 byte)
                     *   1 is bit 1
                     *   0 is bit 0
                     */

                    /* unicode codepoint start */
                    *p = 0xE0;

                    /* print unicode private block header first 4 bits */
                    *p |= FLB_UTILS_FRAGMENT_PRIVATE_BLOCK_DESCRIPTOR >> 4;
                    ++p;

                    /* unicode codepoint middle */
                    *p = 0x80;

                    /* print end of unicode private block header last 4 bits */
                    *p |= ((FLB_UTILS_FRAGMENT_PRIVATE_BLOCK_DESCRIPTOR << 2) & 0x3f);

                    /* print hex fragment first 2 bits */
                    *p |= (tmp[b] >> 6) & 0x03;
                    ++p;

                    /* unicode codepoint middle */
                    *p = 0x80;

                    /* print hex fragment last 6 bits */
                    *p |= tmp[b] & 0x3f;
                    ++p;
                }
            }
        }
        else {
            *p++ = c;
        }
        written = (p - (buf + *off));
    }

    *off += written;

    return FLB_TRUE;
}
int main() {
//size check edit
    size_t written=100;

    int offset;
    size_t size;
    
//flb_msgpack_to_json
    char tmp_buf_ptr[written];
    klee_make_symbolic(tmp_buf_ptr, sizeof(tmp_buf_ptr), "tmp_buf_ptr");  
    tmp_buf_ptr[written-1] = '\0'; 
    for(int i=0;i<written-1;i++)
    klee_assume(tmp_buf_ptr[i]!=0);
  
    //this malloc not null
    size = written*6;
    char event_buf[size];  
    klee_make_symbolic(event_buf, sizeof(event_buf), "event_buf");
    
    offset = 0;
    flb_utils_write_str(event_buf, &offset, size, tmp_buf_ptr, written);
    klee_assume(offset>written*6);
  return offset;
}