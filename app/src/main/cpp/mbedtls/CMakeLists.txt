cmake_minimum_required(VERSION 3.4.1)

set(src_crypto
        ${CMAKE_CURRENT_SOURCE_DIR}/library/aes.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/aesni.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/arc4.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/asn1parse.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/asn1write.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/base64.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/bignum.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/blowfish.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/camellia.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/ccm.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/cipher.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/cipher_wrap.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/cmac.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/ctr_drbg.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/des.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/dhm.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/ecdh.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/ecdsa.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/ecjpake.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/ecp.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/ecp_curves.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/entropy.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/entropy_poll.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/error.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/gcm.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/havege.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/hmac_drbg.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/md.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/md2.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/md4.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/md5.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/md_wrap.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/memory_buffer_alloc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/net_sockets.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/oid.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/padlock.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/pem.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/pk.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/pkcs12.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/pkcs5.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/pkparse.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/pkwrite.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/pk_wrap.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/platform.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/ripemd160.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/rsa.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/sha1.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/sha256.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/sha512.c

        ${CMAKE_CURRENT_SOURCE_DIR}/library/threading.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/timing.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/version.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/version_features.c

        ${CMAKE_CURRENT_SOURCE_DIR}/library/xtea.c
        )
set(src_x509
        ${CMAKE_CURRENT_SOURCE_DIR}/library/certs.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/pkcs11.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/x509.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/x509write_crt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/x509write_csr.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/x509_create.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/x509_crl.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/x509_crt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/x509_csr.c)
set(src_tls
        ${CMAKE_CURRENT_SOURCE_DIR}/library/debug.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/ssl_cache.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/ssl_ciphersuites.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/ssl_cli.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/ssl_cookie.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/ssl_srv.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/ssl_ticket.c
        ${CMAKE_CURRENT_SOURCE_DIR}/library/ssl_tls.c
        )
#-Wdocumentation -Wno-documentation-deprecated-sync
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-declarations -Wmissing-prototypes -Wunreachable-code")
set(mbedtls_static_target "mbedtls")
set(mbedx509_static_target "mbedx509")
set(mbedcrypto_static_target "mbedcrypto")

#set(libs ${libs} ws2_32)

add_library(${mbedcrypto_static_target} STATIC ${src_crypto})
#target_link_libraries(${mbedcrypto_static_target} ${libs})

add_library(${mbedx509_static_target} STATIC ${src_x509})
#target_link_libraries(${mbedx509_static_target} ${libs} ${mbedcrypto_static_target})
target_link_libraries(${mbedx509_static_target} ${mbedcrypto_static_target})

add_library(${mbedtls_static_target} STATIC ${src_tls})
# target_link_libraries(${mbedtls_static_target} ${libs} ${mbedx509_static_target})
target_link_libraries(${mbedtls_static_target} ${mbedx509_static_target})
