#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "miniz.h"
#include "sodium.h"


#ifndef CACERT_PEM_PATH
#define CACERT_PEM_PATH "cacert.pem"
#endif

#ifndef CACERT_H_OUT_PATH
#define CACERT_H_OUT_PATH "cacert.h"
#endif

char* base64Encoded(uint8_t* data, size_t data_len, size_t* out_len) {
  size_t encoded_len = sodium_base64_encoded_len(data_len, sodium_base64_VARIANT_ORIGINAL);
  char* encoded = malloc(encoded_len);
  if (!encoded) {
    return NULL;
  }
  if (sodium_bin2base64(encoded, encoded_len, data, data_len, sodium_base64_VARIANT_ORIGINAL) == NULL) {
    free(encoded);
    return NULL;
  }
  if (out_len) {
    *out_len = encoded_len - 1; // Exclude null terminator
  }
  return encoded;
}

int main(void) {
  sodium_init();
  FILE *fp = fopen(CACERT_PEM_PATH, "r");
  if (!fp) {
    fprintf(stderr, "Failed to open %s\n", CACERT_PEM_PATH);
    return 1;
  }
  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  unsigned char* data = malloc(size);
  if (!data) {
    fprintf(stderr, "Failed to allocate memory for cacert file of: %lu\n", size);
    fclose(fp);
    return 2;
  }
  unsigned long actuallyRead = fread(data, 1, size, fp);
  if (actuallyRead != size) {
    fprintf(stderr, "Failed to read complete cacert file: %lu, but only %lu readed\n", size, actuallyRead);
    free(data);
    fclose(fp);
    return 2;
  }
  fclose(fp);

  mz_ulong compressed_size = compressBound(size);
  unsigned char* compressed = malloc(compressed_size);
  if (!compressed) {
    fprintf(stderr, "Failed to allocate memory for compressed version of cacert of: %lu\n", compressed_size);
    free(data);
    return 3;
  }
  int ret = compress2(compressed, &compressed_size, data, size, Z_BEST_COMPRESSION);
  if (MZ_OK == ret) {
    size_t base64_len = 0;
    char* base64 = base64Encoded(compressed, compressed_size, &base64_len);
    if (base64) {
      FILE *out_fp = fopen(CACERT_H_OUT_PATH, "w");
      if (!out_fp) {
        fprintf(stderr, "Failed to open %s for writing\n", CACERT_H_OUT_PATH);
      } else {
        fprintf(out_fp, "#ifndef CACERT_H\n#define CACERT_H\n\n");
        fprintf(out_fp, "const char* const cacert_pem_base64 =\n");

        // overcome msvc compiler limit
        size_t chunk = 12000;
        for (size_t i = 0; i < base64_len; i += chunk) {
          size_t len = chunk;
          if (i + len > base64_len) {
            len = base64_len - i;
          }

          fprintf(out_fp, "\"%.*s\"\n", (int)len, base64 + i);
        }

        fprintf(out_fp, ";\n");
        fprintf(out_fp, "const unsigned long cacert_pem_base64_len = %lu;\n", base64_len);
        fprintf(out_fp, "const unsigned long cacert_pem_size = %lu;\n", size);
        fprintf(out_fp, "\n#endif // CACERT_H\n");
        fclose(out_fp);
      }
      free(base64);
    } else {
      fprintf(stderr, "Base64 encoding failed\n");
    }
  }
  free(data);
  free(compressed);
  if(ret != MZ_OK) {
    fprintf(stderr, "compress failed\n");
    return 1;
  }
  return 0;
}