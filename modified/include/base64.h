
void base64_encode(const unsigned char *data, 
                    char *encoded_data,
                    size_t input_length,
                    size_t *output_length);

void base64_decode(const char *data,
                   unsigned char *decoded_data,
                   size_t input_length,
                   size_t *output_length);
