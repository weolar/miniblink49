/* ssl.h
 *
 * Copyright (C) 2006-2016 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */



/*  ssl.h defines wolfssl_openssl compatibility layer
 *
 */


#ifndef WOLFSSL_OPENSSL_H_
#define WOLFSSL_OPENSSL_H_

/* wolfssl_openssl compatibility layer */
#include <wolfssl/ssl.h>

#include <wolfssl/openssl/evp.h>

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef _WIN32
    /* wincrypt.h clashes */
    #undef X509_NAME
#endif

#ifdef WOLFSSL_UTASKER
    /* tcpip.h clashes */
    #undef ASN1_INTEGER
#endif


typedef WOLFSSL          SSL;
typedef WOLFSSL_SESSION  SSL_SESSION;
typedef WOLFSSL_METHOD   SSL_METHOD;
typedef WOLFSSL_CTX      SSL_CTX;

typedef WOLFSSL_X509       X509;
typedef WOLFSSL_X509_NAME  X509_NAME;
typedef WOLFSSL_X509_CHAIN X509_CHAIN;


/* redeclare guard */
#define WOLFSSL_TYPES_DEFINED


typedef WOLFSSL_EVP_PKEY               EVP_PKEY;
typedef WOLFSSL_BIO                    BIO;
typedef WOLFSSL_BIO_METHOD             BIO_METHOD;
typedef WOLFSSL_CIPHER                 SSL_CIPHER;
typedef WOLFSSL_X509_LOOKUP            X509_LOOKUP;
typedef WOLFSSL_X509_LOOKUP_METHOD     X509_LOOKUP_METHOD;
typedef WOLFSSL_X509_CRL               X509_CRL;
typedef WOLFSSL_X509_EXTENSION         X509_EXTENSION;
typedef WOLFSSL_ASN1_TIME              ASN1_TIME;
typedef WOLFSSL_ASN1_INTEGER           ASN1_INTEGER;
typedef WOLFSSL_ASN1_OBJECT            ASN1_OBJECT;
typedef WOLFSSL_ASN1_STRING            ASN1_STRING;
typedef WOLFSSL_dynlock_value          CRYPTO_dynlock_value;
typedef WOLFSSL_BUF_MEM                BUF_MEM;


/* GENERAL_NAME and BASIC_CONSTRAINTS structs may need implemented as
 * compatibility layer expands. For now treating them as an ASN1_OBJECT */
typedef WOLFSSL_ASN1_OBJECT GENERAL_NAME;
typedef WOLFSSL_ASN1_OBJECT BASIC_CONSTRAINTS;

#define ASN1_UTCTIME         WOLFSSL_ASN1_TIME
#define ASN1_GENERALIZEDTIME WOLFSSL_ASN1_TIME

typedef WOLFSSL_MD4_CTX        MD4_CTX;
typedef WOLFSSL_COMP_METHOD    COMP_METHOD;
typedef WOLFSSL_X509_REVOKED   X509_REVOKED;
typedef WOLFSSL_X509_OBJECT    X509_OBJECT;
typedef WOLFSSL_X509_STORE     X509_STORE;
typedef WOLFSSL_X509_STORE_CTX X509_STORE_CTX;

#define CRYPTO_free   XFREE
#define CRYPTO_malloc XMALLOC

#define SSL_get_client_random(ssl,out,outSz) \
                                  wolfSSL_get_client_random((ssl),(out),(outSz))
#define SSL_get_cipher_list(ctx,i)         wolfSSL_get_cipher_list_ex((ctx),(i))
#define SSL_get_cipher_name(ctx)           wolfSSL_get_cipher((ctx))
#define SSL_get_shared_ciphers(ctx,buf,len) \
                                   wolfSSL_get_shared_ciphers((ctx),(buf),(len))

#define ERR_print_errors_fp(file) wolfSSL_ERR_dump_errors_fp((file))

/* at the moment only returns ok */
#define SSL_get_verify_result         wolfSSL_get_verify_result
#define SSL_get_verify_mode           wolfSSL_SSL_get_mode
#define SSL_get_verify_depth          wolfSSL_get_verify_depth
#define SSL_CTX_get_verify_mode       wolfSSL_CTX_get_verify_mode
#define SSL_CTX_get_verify_depth      wolfSSL_CTX_get_verify_depth
#define SSL_get_certificate           wolfSSL_get_certificate
#define SSL_use_certificate           wolfSSL_use_certificate
#define SSL_use_certificate_ASN1      wolfSSL_use_certificate_ASN1

#define SSL_use_PrivateKey         wolfSSL_use_PrivateKey
#define SSL_use_PrivateKey_ASN1    wolfSSL_use_PrivateKey_ASN1
#define SSL_use_RSAPrivateKey_ASN1 wolfSSL_use_RSAPrivateKey_ASN1
#define SSL_get_privatekey         wolfSSL_get_privatekey

#define SSLv23_method       wolfSSLv23_method
#define SSLv3_server_method wolfSSLv3_server_method
#define SSLv3_client_method wolfSSLv3_client_method
#define TLSv1_server_method wolfTLSv1_server_method
#define TLSv1_client_method wolfTLSv1_client_method
#define TLSv1_1_server_method wolfTLSv1_1_server_method
#define TLSv1_1_client_method wolfTLSv1_1_client_method
#define TLSv1_2_server_method wolfTLSv1_2_server_method
#define TLSv1_2_client_method wolfTLSv1_2_client_method

#ifdef WOLFSSL_DTLS
    #define DTLSv1_client_method wolfDTLSv1_client_method
    #define DTLSv1_server_method wolfDTLSv1_server_method
    #define DTLSv1_2_client_method wolfDTLSv1_2_client_method
    #define DTLSv1_2_server_method wolfDTLSv1_2_server_method
#endif


#ifndef NO_FILESYSTEM
    #define SSL_CTX_use_certificate_file wolfSSL_CTX_use_certificate_file
    #define SSL_CTX_use_PrivateKey_file wolfSSL_CTX_use_PrivateKey_file
    #define SSL_CTX_load_verify_locations wolfSSL_CTX_load_verify_locations
    #define SSL_CTX_use_certificate_chain_file wolfSSL_CTX_use_certificate_chain_file
    #define SSL_CTX_use_RSAPrivateKey_file wolfSSL_CTX_use_RSAPrivateKey_file

    #define SSL_use_certificate_file wolfSSL_use_certificate_file
    #define SSL_use_PrivateKey_file wolfSSL_use_PrivateKey_file
    #define SSL_use_certificate_chain_file wolfSSL_use_certificate_chain_file
    #define SSL_use_RSAPrivateKey_file wolfSSL_use_RSAPrivateKey_file
#endif

#define SSL_CTX_new wolfSSL_CTX_new
#define SSL_new     wolfSSL_new
#define SSL_set_fd  wolfSSL_set_fd
#define SSL_get_fd  wolfSSL_get_fd
#define SSL_connect wolfSSL_connect
#define SSL_clear   wolfSSL_clear
#define SSL_state   wolfSSL_state

#define SSL_write    wolfSSL_write
#define SSL_read     wolfSSL_read
#define SSL_peek     wolfSSL_peek
#define SSL_accept   wolfSSL_accept
#define SSL_CTX_free wolfSSL_CTX_free
#define SSL_free     wolfSSL_free
#define SSL_shutdown wolfSSL_shutdown

#define SSL_CTX_set_quiet_shutdown wolfSSL_CTX_set_quiet_shutdown
#define SSL_set_quiet_shutdown wolfSSL_set_quiet_shutdown
#define SSL_get_error wolfSSL_get_error
#define SSL_set_session wolfSSL_set_session
#define SSL_get_session wolfSSL_get_session
#define SSL_flush_sessions wolfSSL_flush_sessions
/* assume unlimited temporarily */
#define SSL_CTX_get_session_cache_mode(ctx) 0

#define SSL_CTX_set_verify wolfSSL_CTX_set_verify
#define SSL_set_verify wolfSSL_set_verify
#define SSL_pending wolfSSL_pending
#define SSL_load_error_strings wolfSSL_load_error_strings
#define SSL_library_init wolfSSL_library_init
#define SSL_CTX_set_session_cache_mode wolfSSL_CTX_set_session_cache_mode
#define SSL_CTX_set_cipher_list wolfSSL_CTX_set_cipher_list
#define SSL_set_cipher_list     wolfSSL_set_cipher_list

#define ERR_error_string wolfSSL_ERR_error_string
#define ERR_error_string_n wolfSSL_ERR_error_string_n
#define ERR_reason_error_string wolfSSL_ERR_reason_error_string

#define SSL_set_ex_data wolfSSL_set_ex_data
#define SSL_get_shutdown wolfSSL_get_shutdown
#define SSL_set_rfd wolfSSL_set_rfd
#define SSL_set_wfd wolfSSL_set_wfd
#define SSL_set_shutdown wolfSSL_set_shutdown
#define SSL_set_session_id_context wolfSSL_set_session_id_context
#define SSL_set_connect_state wolfSSL_set_connect_state
#define SSL_set_accept_state wolfSSL_set_accept_state
#define SSL_session_reused wolfSSL_session_reused
#define SSL_SESSION_free wolfSSL_SESSION_free
#define SSL_is_init_finished wolfSSL_is_init_finished

#define SSL_get_version        wolfSSL_get_version
#define SSL_get_current_cipher wolfSSL_get_current_cipher

/* use wolfSSL_get_cipher_name for its return format */
#define SSL_get_cipher         wolfSSL_get_cipher_name
#define SSL_CIPHER_description wolfSSL_CIPHER_description
#define SSL_CIPHER_get_name    wolfSSL_CIPHER_get_name
#define SSL_get1_session       wolfSSL_get1_session

#define SSL_get_keyblock_size wolfSSL_get_keyblock_size
#define SSL_get_keys          wolfSSL_get_keys
#define SSL_SESSION_get_master_key        wolfSSL_SESSION_get_master_key
#define SSL_SESSION_get_master_key_length wolfSSL_SESSION_get_master_key_length

#define X509_NAME_get_text_by_NID wolfSSL_X509_NAME_get_text_by_NID
#define X509_get_ext_d2i wolfSSL_X509_get_ext_d2i
#define X509_digest wolfSSL_X509_digest
#define X509_free wolfSSL_X509_free
#define OPENSSL_free wolfSSL_OPENSSL_free

#define OCSP_parse_url wolfSSL_OCSP_parse_url
#define SSLv23_client_method wolfSSLv23_client_method
#define SSLv2_client_method wolfSSLv2_client_method
#define SSLv2_server_method wolfSSLv2_server_method

#define MD4_Init wolfSSL_MD4_Init
#define MD4_Update  wolfSSL_MD4_Update
#define MD4_Final wolfSSL_MD4_Final

#define BIO_new      wolfSSL_BIO_new
#define BIO_free     wolfSSL_BIO_free
#define BIO_free_all wolfSSL_BIO_free_all
#define BIO_nread0   wolfSSL_BIO_nread0
#define BIO_nread    wolfSSL_BIO_nread
#define BIO_read     wolfSSL_BIO_read
#define BIO_nwrite0  wolfSSL_BIO_nwrite0
#define BIO_nwrite   wolfSSL_BIO_nwrite
#define BIO_write    wolfSSL_BIO_write
#define BIO_push     wolfSSL_BIO_push
#define BIO_pop      wolfSSL_BIO_pop
#define BIO_flush    wolfSSL_BIO_flush
#define BIO_pending  wolfSSL_BIO_pending

#define BIO_get_mem_data wolfSSL_BIO_get_mem_data
#define BIO_new_mem_buf  wolfSSL_BIO_new_mem_buf

#define BIO_f_buffer              wolfSSL_BIO_f_buffer
#define BIO_set_write_buffer_size wolfSSL_BIO_set_write_buffer_size
#define BIO_f_ssl                 wolfSSL_BIO_f_ssl
#define BIO_new_socket            wolfSSL_BIO_new_socket
#define SSL_set_bio               wolfSSL_set_bio
#define BIO_eof                   wolfSSL_BIO_eof
#define BIO_set_ss                wolfSSL_BIO_set_ss

#define BIO_s_mem     wolfSSL_BIO_s_mem
#define BIO_f_base64  wolfSSL_BIO_f_base64
#define BIO_set_flags wolfSSL_BIO_set_flags

#define OpenSSL_add_all_digests()
#define OpenSSL_add_all_algorithms wolfSSL_add_all_algorithms
#define SSLeay_add_ssl_algorithms  wolfSSL_add_all_algorithms
#define SSLeay_add_all_algorithms  wolfSSL_add_all_algorithms

#define RAND_screen     wolfSSL_RAND_screen
#define RAND_file_name  wolfSSL_RAND_file_name
#define RAND_write_file wolfSSL_RAND_write_file
#define RAND_load_file  wolfSSL_RAND_load_file
#define RAND_egd        wolfSSL_RAND_egd
#define RAND_seed       wolfSSL_RAND_seed
#define RAND_add        wolfSSL_RAND_add

#define COMP_zlib                       wolfSSL_COMP_zlib
#define COMP_rle                        wolfSSL_COMP_rle
#define SSL_COMP_add_compression_method wolfSSL_COMP_add_compression_method

#define SSL_get_ex_new_index wolfSSL_get_ex_new_index

#define CRYPTO_set_id_callback wolfSSL_set_id_callback
#define CRYPTO_set_locking_callback wolfSSL_set_locking_callback
#define CRYPTO_set_dynlock_create_callback wolfSSL_set_dynlock_create_callback
#define CRYPTO_set_dynlock_lock_callback wolfSSL_set_dynlock_lock_callback
#define CRYPTO_set_dynlock_destroy_callback wolfSSL_set_dynlock_destroy_callback
#define CRYPTO_num_locks wolfSSL_num_locks


#  define CRYPTO_LOCK             1
#  define CRYPTO_UNLOCK           2
#  define CRYPTO_READ             4
#  define CRYPTO_WRITE            8

#define X509_STORE_CTX_get_current_cert wolfSSL_X509_STORE_CTX_get_current_cert
#define X509_STORE_add_cert             wolfSSL_X509_STORE_add_cert
#define X509_STORE_set_flags            wolfSSL_X509_STORE_set_flags
#define X509_STORE_CTX_get_chain        wolfSSL_X509_STORE_CTX_get_chain
#define X509_STORE_CTX_get_error wolfSSL_X509_STORE_CTX_get_error
#define X509_STORE_CTX_get_error_depth wolfSSL_X509_STORE_CTX_get_error_depth

#define X509_NAME_oneline             wolfSSL_X509_NAME_oneline
#define X509_get_issuer_name          wolfSSL_X509_get_issuer_name
#define X509_get_subject_name         wolfSSL_X509_get_subject_name
#define X509_verify_cert_error_string wolfSSL_X509_verify_cert_error_string

#define X509_LOOKUP_add_dir wolfSSL_X509_LOOKUP_add_dir
#define X509_LOOKUP_load_file wolfSSL_X509_LOOKUP_load_file
#define X509_LOOKUP_hash_dir wolfSSL_X509_LOOKUP_hash_dir
#define X509_LOOKUP_file wolfSSL_X509_LOOKUP_file

#define X509_STORE_add_lookup wolfSSL_X509_STORE_add_lookup
#define X509_STORE_new wolfSSL_X509_STORE_new
#define X509_STORE_get_by_subject wolfSSL_X509_STORE_get_by_subject
#define X509_STORE_CTX_init wolfSSL_X509_STORE_CTX_init
#define X509_STORE_CTX_cleanup wolfSSL_X509_STORE_CTX_cleanup

#define X509_CRL_get_lastUpdate wolfSSL_X509_CRL_get_lastUpdate
#define X509_CRL_get_nextUpdate wolfSSL_X509_CRL_get_nextUpdate

#define X509_get_pubkey           wolfSSL_X509_get_pubkey
#define X509_CRL_verify           wolfSSL_X509_CRL_verify
#define X509_STORE_CTX_set_error  wolfSSL_X509_STORE_CTX_set_error
#define X509_OBJECT_free_contents wolfSSL_X509_OBJECT_free_contents
#define EVP_PKEY_new              wolfSSL_PKEY_new
#define EVP_PKEY_free             wolfSSL_EVP_PKEY_free
#define EVP_PKEY_type             wolfSSL_EVP_PKEY_type
#define EVP_PKEY_base_id          wolfSSL_EVP_PKEY_base_id
#define X509_cmp_current_time     wolfSSL_X509_cmp_current_time
#define sk_X509_REVOKED_num       wolfSSL_sk_X509_REVOKED_num
#define X509_CRL_get_REVOKED      wolfSSL_X509_CRL_get_REVOKED
#define sk_X509_REVOKED_value     wolfSSL_sk_X509_REVOKED_value
#define X509_get_notBefore(cert)  (ASN1_TIME*)wolfSSL_X509_notBefore((cert))
#define X509_get_notAfter(cert)   (ASN1_TIME*)wolfSSL_X509_notAfter((cert))


#define X509_get_serialNumber wolfSSL_X509_get_serialNumber

#define ASN1_TIME_print              wolfSSL_ASN1_TIME_print
#define ASN1_GENERALIZEDTIME_print   wolfSSL_ASN1_GENERALIZEDTIME_print

#define ASN1_INTEGER_cmp wolfSSL_ASN1_INTEGER_cmp
#define ASN1_INTEGER_get wolfSSL_ASN1_INTEGER_get
#define ASN1_INTEGER_to_BN wolfSSL_ASN1_INTEGER_to_BN
#define ASN1_STRING_to_UTF8 wolfSSL_ASN1_STRING_to_UTF8

#define SSL_load_client_CA_file wolfSSL_load_client_CA_file

#define SSL_CTX_get_client_CA_list         wolfSSL_SSL_CTX_get_client_CA_list
#define SSL_CTX_set_client_CA_list         wolfSSL_CTX_set_client_CA_list
#define SSL_CTX_set_cert_store             wolfSSL_CTX_set_cert_store
#define SSL_CTX_get_cert_store             wolfSSL_CTX_get_cert_store
#define X509_STORE_CTX_get_ex_data         wolfSSL_X509_STORE_CTX_get_ex_data
#define SSL_get_ex_data_X509_STORE_CTX_idx wolfSSL_get_ex_data_X509_STORE_CTX_idx
#define SSL_get_ex_data wolfSSL_get_ex_data

#define SSL_CTX_set_default_passwd_cb_userdata wolfSSL_CTX_set_default_passwd_cb_userdata
#define SSL_CTX_set_default_passwd_cb wolfSSL_CTX_set_default_passwd_cb

#define SSL_CTX_set_timeout(ctx, to) wolfSSL_CTX_set_timeout(ctx, (unsigned int) to)
#define SSL_CTX_set_info_callback wolfSSL_CTX_set_info_callback

#define ERR_peek_error wolfSSL_ERR_peek_error
#define ERR_peek_last_error_line  wolfSSL_ERR_peek_last_error_line
#define ERR_peek_errors_fp         wolfSSL_ERR_peek_errors_fp
#define ERR_GET_REASON wolfSSL_ERR_GET_REASON

#define SSL_alert_type_string wolfSSL_alert_type_string
#define SSL_alert_desc_string wolfSSL_alert_desc_string
#define SSL_state_string wolfSSL_state_string

#define RSA_free wolfSSL_RSA_free
#define RSA_generate_key wolfSSL_RSA_generate_key
#define SSL_CTX_set_tmp_rsa_callback wolfSSL_CTX_set_tmp_rsa_callback

#define PEM_def_callback wolfSSL_PEM_def_callback

#define SSL_CTX_sess_accept wolfSSL_CTX_sess_accept
#define SSL_CTX_sess_connect wolfSSL_CTX_sess_connect
#define SSL_CTX_sess_accept_good wolfSSL_CTX_sess_accept_good
#define SSL_CTX_sess_connect_good wolfSSL_CTX_sess_connect_good
#define SSL_CTX_sess_accept_renegotiate wolfSSL_CTX_sess_accept_renegotiate
#define SSL_CTX_sess_connect_renegotiate wolfSSL_CTX_sess_connect_renegotiate
#define SSL_CTX_sess_hits wolfSSL_CTX_sess_hits
#define SSL_CTX_sess_cb_hits wolfSSL_CTX_sess_cb_hits
#define SSL_CTX_sess_cache_full wolfSSL_CTX_sess_cache_full
#define SSL_CTX_sess_misses wolfSSL_CTX_sess_misses
#define SSL_CTX_sess_timeouts wolfSSL_CTX_sess_timeouts
#define SSL_CTX_sess_number wolfSSL_CTX_sess_number
#define SSL_CTX_sess_get_cache_size wolfSSL_CTX_sess_get_cache_size


#define SSL_DEFAULT_CIPHER_LIST WOLFSSL_DEFAULT_CIPHER_LIST
#define RSA_F4 WOLFSSL_RSA_F4

#define SSL_CTX_set_psk_client_callback wolfSSL_CTX_set_psk_client_callback
#define SSL_set_psk_client_callback wolfSSL_set_psk_client_callback

#define SSL_get_psk_identity_hint wolfSSL_get_psk_identity_hint
#define SSL_get_psk_identity wolfSSL_get_psk_identity

#define SSL_CTX_use_psk_identity_hint wolfSSL_CTX_use_psk_identity_hint
#define SSL_use_psk_identity_hint wolfSSL_use_psk_identity_hint

#define SSL_CTX_set_psk_server_callback wolfSSL_CTX_set_psk_server_callback
#define SSL_set_psk_server_callback wolfSSL_set_psk_server_callback

#define ERR_get_error_line_data wolfSSL_ERR_get_error_line_data

#define ERR_get_error wolfSSL_ERR_get_error
#define ERR_clear_error wolfSSL_ERR_clear_error

#define RAND_status wolfSSL_RAND_status
#define RAND_bytes wolfSSL_RAND_bytes
#define SSLv23_server_method wolfSSLv23_server_method
#define SSL_CTX_set_options wolfSSL_CTX_set_options
#define SSL_CTX_check_private_key wolfSSL_CTX_check_private_key

#define ERR_free_strings wolfSSL_ERR_free_strings
#define ERR_remove_state wolfSSL_ERR_remove_state
#define EVP_cleanup wolfSSL_EVP_cleanup

#define CRYPTO_cleanup_all_ex_data wolfSSL_cleanup_all_ex_data
#define SSL_CTX_set_mode wolfSSL_CTX_set_mode
#define SSL_CTX_get_mode wolfSSL_CTX_get_mode
#define SSL_CTX_set_default_read_ahead wolfSSL_CTX_set_default_read_ahead

#define SSL_CTX_sess_set_cache_size wolfSSL_CTX_sess_set_cache_size
#define SSL_CTX_set_default_verify_paths wolfSSL_CTX_set_default_verify_paths

#define SSL_CTX_set_session_id_context wolfSSL_CTX_set_session_id_context
#define SSL_get_peer_certificate wolfSSL_get_peer_certificate

#define SSL_want_read wolfSSL_want_read
#define SSL_want_write wolfSSL_want_write

#define BIO_prf wolfSSL_BIO_prf
#define ASN1_UTCTIME_pr wolfSSL_ASN1_UTCTIME_pr

#define sk_num wolfSSL_sk_num
#define sk_value wolfSSL_sk_value
#define sk_X509_pop  wolfSSL_sk_X509_pop
#define sk_X509_free wolfSSL_sk_X509_free
#define d2i_X509_bio wolfSSL_d2i_X509_bio

#define SSL_CTX_get_ex_data wolfSSL_CTX_get_ex_data
#define SSL_CTX_set_ex_data wolfSSL_CTX_set_ex_data
#define SSL_CTX_sess_set_get_cb wolfSSL_CTX_sess_set_get_cb
#define SSL_CTX_sess_set_new_cb wolfSSL_CTX_sess_set_new_cb
#define SSL_CTX_sess_set_remove_cb wolfSSL_CTX_sess_set_remove_cb

#define i2d_SSL_SESSION wolfSSL_i2d_SSL_SESSION
#define d2i_SSL_SESSION wolfSSL_d2i_SSL_SESSION
#define SSL_SESSION_set_timeout wolfSSL_SSL_SESSION_set_timeout
#define SSL_SESSION_get_timeout wolfSSL_SESSION_get_timeout
#define SSL_SESSION_get_time wolfSSL_SESSION_get_time
#define SSL_CTX_get_ex_new_index wolfSSL_CTX_get_ex_new_index
#define PEM_read_bio_X509 wolfSSL_PEM_read_bio_X509
#define PEM_read_bio_X509_AUX wolfSSL_PEM_read_bio_X509_AUX

/*#if OPENSSL_API_COMPAT < 0x10100000L*/
#define CONF_modules_free()
#define ENGINE_cleanup()
#define HMAC_CTX_cleanup wolfSSL_HMAC_cleanup
#define SSL_CTX_need_tmp_RSA(ctx)            0
#define SSL_CTX_set_tmp_rsa(ctx,rsa)         1
#define SSL_need_tmp_RSA(ssl)                0
#define SSL_set_tmp_rsa(ssl,rsa)             1
/*#endif*/
#define CONF_modules_unload(a)

#define SSL_get_hit wolfSSL_session_reused

/* yassl had set the default to be 500 */
#define SSL_get_default_timeout(ctx) 500

/* Lighthttp compatibility */

#if defined(HAVE_LIGHTY)  || defined(WOLFSSL_MYSQL_COMPATIBLE) || \
    defined(HAVE_STUNNEL) || defined(WOLFSSL_NGINX) || \
    defined(HAVE_POCO_LIB) || defined(WOLFSSL_HAPROXY) 
typedef WOLFSSL_X509_NAME_ENTRY X509_NAME_ENTRY;

#define X509_NAME_free wolfSSL_X509_NAME_free
#define SSL_CTX_use_certificate wolfSSL_CTX_use_certificate
#define SSL_CTX_use_PrivateKey wolfSSL_CTX_use_PrivateKey
#define BIO_read_filename wolfSSL_BIO_read_filename
#define BIO_s_file wolfSSL_BIO_s_file
#define OBJ_nid2sn wolfSSL_OBJ_nid2sn
#define OBJ_obj2nid wolfSSL_OBJ_obj2nid
#define OBJ_sn2nid wolfSSL_OBJ_sn2nid
#define SSL_CTX_set_verify_depth wolfSSL_CTX_set_verify_depth
#define SSL_set_verify_depth wolfSSL_set_verify_depth
#define SSL_get_app_data wolfSSL_get_app_data
#define SSL_set_app_data wolfSSL_set_app_data
#define X509_NAME_entry_count wolfSSL_X509_NAME_entry_count
#define X509_NAME_ENTRY_get_object wolfSSL_X509_NAME_ENTRY_get_object
#define X509_NAME_get_entry wolfSSL_X509_NAME_get_entry
#define ASN1_STRING_data wolfSSL_ASN1_STRING_data
#define ASN1_STRING_length wolfSSL_ASN1_STRING_length
#define X509_NAME_get_index_by_NID wolfSSL_X509_NAME_get_index_by_NID
#define X509_NAME_ENTRY_get_data wolfSSL_X509_NAME_ENTRY_get_data
#define sk_X509_NAME_pop_free  wolfSSL_sk_X509_NAME_pop_free
#define SHA1 wolfSSL_SHA1
#define X509_check_private_key wolfSSL_X509_check_private_key
#define SSL_dup_CA_list wolfSSL_dup_CA_list

#define NID_commonName 0x03 /* matchs ASN_COMMON_NAME in asn.h */

#define OBJ_nid2ln wolfSSL_OBJ_nid2ln
#define OBJ_txt2nid wolfSSL_OBJ_txt2nid
#define PEM_read_bio_DHparams wolfSSL_PEM_read_bio_DHparams
#define PEM_read_bio_DSAparams wolfSSL_PEM_read_bio_DSAparams
#define PEM_write_bio_X509 wolfSSL_PEM_write_bio_X509


#ifdef WOLFSSL_HAPROXY
#define SSL_get_rbio                      wolfSSL_SSL_get_rbio
#define SSL_get_wbio                      wolfSSL_SSL_get_wbio
#define SSL_do_handshake                  wolfSSL_SSL_do_handshake
#define SSL_get_ciphers(x)                wolfSSL_get_ciphers_compat(x)
#define SSL_SESSION_get_id                wolfSSL_SESSION_get_id
#define ASN1_STRING_get0_data             wolfSSL_ASN1_STRING_data
#define SSL_get_cipher_bits(s,np)         wolfSSL_CIPHER_get_bits(SSL_get_current_cipher(s),np)
#define sk_SSL_CIPHER_num                 wolfSSL_sk_SSL_CIPHER_num
#define sk_SSL_COMP_zero                  wolfSSL_sk_SSL_COMP_zero
#define sk_SSL_CIPHER_value               wolfSSL_sk_SSL_CIPHER_value
#endif /* WOLFSSL_HAPROXY */
#endif /* HAVE_STUNNEL || HAVE_LIGHTY || WOLFSSL_MYSQL_COMPATIBLE || WOLFSSL_NGINX || HAVE_POCO_LIB || WOLFSSL_HAPROXY */

#define SSL_CTX_set_tmp_dh wolfSSL_CTX_set_tmp_dh

#define BIO_new_file        wolfSSL_BIO_new_file
#define BIO_ctrl            wolfSSL_BIO_ctrl
#define BIO_ctrl_pending    wolfSSL_BIO_ctrl_pending
#define BIO_get_mem_ptr     wolfSSL_BIO_get_mem_ptr
#define BIO_int_ctrl        wolfSSL_BIO_int_ctrl
#define BIO_reset           wolfSSL_BIO_reset
#define BIO_s_file          wolfSSL_BIO_s_file
#define BIO_s_bio           wolfSSL_BIO_s_bio
#define BIO_s_socket        wolfSSL_BIO_s_socket
#define BIO_set_fd          wolfSSL_BIO_set_fd
#define BIO_ctrl_reset_read_request wolfSSL_BIO_ctrl_reset_read_request

#define BIO_set_write_buf_size wolfSSL_BIO_set_write_buf_size
#define BIO_make_bio_pair   wolfSSL_BIO_make_bio_pair

#define BIO_set_fp          wolfSSL_BIO_set_fp
#define BIO_get_fp          wolfSSL_BIO_get_fp
#define BIO_seek            wolfSSL_BIO_seek
#define BIO_write_filename  wolfSSL_BIO_write_filename
#define BIO_set_mem_eof_return wolfSSL_BIO_set_mem_eof_return

#define SSL_set_options      wolfSSL_set_options
#define SSL_get_options      wolfSSL_get_options
#define SSL_set_tmp_dh       wolfSSL_set_tmp_dh
#define SSL_clear_num_renegotiations    wolfSSL_clear_num_renegotiations
#define SSL_total_renegotiations       wolfSSL_total_renegotiations
#define SSL_set_tlsext_debug_arg        wolfSSL_set_tlsext_debug_arg
#define SSL_set_tlsext_status_type      wolfSSL_set_tlsext_status_type
#define SSL_set_tlsext_status_exts      wolfSSL_set_tlsext_status_exts
#define SSL_get_tlsext_status_ids       wolfSSL_get_tlsext_status_ids
#define SSL_set_tlsext_status_ids       wolfSSL_set_tlsext_status_ids
#define SSL_get_tlsext_status_ocsp_resp wolfSSL_get_tlsext_status_ocsp_resp
#define SSL_set_tlsext_status_ocsp_resp wolfSSL_set_tlsext_status_ocsp_resp

#define SSL_CTX_add_extra_chain_cert wolfSSL_CTX_add_extra_chain_cert
#define SSL_CTX_get_read_ahead wolfSSL_CTX_get_read_ahead
#define SSL_CTX_set_read_ahead wolfSSL_CTX_set_read_ahead
#define SSL_CTX_set_tlsext_status_arg wolfSSL_CTX_set_tlsext_status_arg
#define SSL_CTX_set_tlsext_opaque_prf_input_callback_arg \
                   wolfSSL_CTX_set_tlsext_opaque_prf_input_callback_arg
#define SSL_get_server_random wolfSSL_get_server_random

#define SSL_get_tlsext_status_exts wolfSSL_get_tlsext_status_exts

#define BIO_C_SET_FILE_PTR                      106
#define BIO_C_GET_FILE_PTR                      107
#define BIO_C_SET_FILENAME                      108
#define BIO_C_FILE_SEEK                         128
#define BIO_C_SET_BUF_MEM_EOF_RETURN            130
#define BIO_C_SET_WRITE_BUF_SIZE                136
#define BIO_C_MAKE_BIO_PAIR                     138

#define BIO_CTRL_RESET          1
#define BIO_CTRL_INFO           3
#define BIO_CTRL_FLUSH          11
#define BIO_CLOSE               0x01
#define BIO_FP_WRITE            0x04

#define SSL_CTRL_CLEAR_NUM_RENEGOTIATIONS         11
#define SSL_CTRL_GET_TOTAL_RENEGOTIATIONS         12
#define SSL_CTRL_SET_TMP_DH                       3
#define SSL_CTRL_SET_TLSEXT_DEBUG_ARG             57
#define SSL_CTRL_SET_TLSEXT_STATUS_REQ_TYPE       65
#define SSL_CTRL_GET_TLSEXT_STATUS_REQ_EXTS       66
#define SSL_CTRL_SET_TLSEXT_STATUS_REQ_EXTS       67
#define SSL_CTRL_GET_TLSEXT_STATUS_REQ_IDS        68
#define SSL_CTRL_SET_TLSEXT_STATUS_REQ_IDS        69
#define SSL_CTRL_GET_TLSEXT_STATUS_REQ_OCSP_RESP  70
#define SSL_CTRL_SET_TLSEXT_STATUS_REQ_OCSP_RESP  71

#define SSL_CTRL_SET_TMP_DH                     3
#define SSL_CTRL_EXTRA_CHAIN_CERT               14

#define SSL_CTRL_SET_SESS_CACHE_SIZE            42
#define SSL_CTRL_GET_READ_AHEAD                 40
#define SSL_CTRL_SET_READ_AHEAD                 41

#define SSL_CTRL_SET_TLSEXT_STATUS_REQ_CB       63
#define SSL_CTRL_SET_TLSEXT_STATUS_REQ_CB_ARG   64

#define SSL_CTRL_GET_EXTRA_CHAIN_CERTS          82

#define SSL_ctrl     wolfSSL_ctrl
#define SSL_CTX_ctrl wolfSSL_CTX_ctrl

#define X509_V_FLAG_CRL_CHECK     WOLFSSL_CRL_CHECK
#define X509_V_FLAG_CRL_CHECK_ALL WOLFSSL_CRL_CHECKALL

#if defined(HAVE_STUNNEL) || defined(WOLFSSL_NGINX)
#include <wolfssl/openssl/asn1.h>

#define SSL2_VERSION                     0x0002
#define SSL3_VERSION                     0x0300
#define TLS1_VERSION                     0x0301
#define DTLS1_VERSION                    0xFEFF
#define SSL23_ST_SR_CLNT_HELLO_A        (0x210|0x2000)
#define SSL3_ST_SR_CLNT_HELLO_A         (0x110|0x2000)
#define ASN1_STRFLGS_ESC_MSB             4
#define X509_V_ERR_CERT_REJECTED         28

#define SSL_alert_desc_string_long       wolfSSL_alert_desc_string_long
#define SSL_alert_type_string_long       wolfSSL_alert_type_string_long
#define SSL_CIPHER_get_bits              wolfSSL_CIPHER_get_bits
#define sk_X509_NAME_num                 wolfSSL_sk_X509_NAME_num
#define sk_X509_num                      wolfSSL_sk_X509_num
#define X509_NAME_print_ex               wolfSSL_X509_NAME_print_ex
#define X509_get0_pubkey_bitstr          wolfSSL_X509_get0_pubkey_bitstr
#define SSL_CTX_get_options              wolfSSL_CTX_get_options

#define SSL_CTX_flush_sessions           wolfSSL_flush_sessions
#define SSL_CTX_add_session              wolfSSL_CTX_add_session
#define SSL_get_SSL_CTX                  wolfSSL_get_SSL_CTX
#define SSL_version                      wolfSSL_version
#define SSL_get_state                    wolfSSL_get_state
#define SSL_state_string_long            wolfSSL_state_string_long
#define SSL_get_peer_cert_chain          wolfSSL_get_peer_cert_chain
#define sk_X509_NAME_value               wolfSSL_sk_X509_NAME_value
#define sk_X509_value                    wolfSSL_sk_X509_value
#define SSL_SESSION_get_ex_data          wolfSSL_SESSION_get_ex_data
#define SSL_SESSION_set_ex_data          wolfSSL_SESSION_set_ex_data
#define SSL_SESSION_get_ex_new_index     wolfSSL_SESSION_get_ex_new_index
#define SSL_SESSION_get_id               wolfSSL_SESSION_get_id
#define CRYPTO_dynlock_value             WOLFSSL_dynlock_value
typedef WOLFSSL_ASN1_BIT_STRING    ASN1_BIT_STRING;
#define X509_STORE_get1_certs            wolfSSL_X509_STORE_get1_certs
#define sk_X509_pop_free                 wolfSSL_sk_X509_pop_free

#define SSL_TLSEXT_ERR_OK                    0
#define SSL_TLSEXT_ERR_ALERT_FATAL           alert_fatal
#define SSL_TLSEXT_ERR_NOACK                 alert_warning
#define TLSEXT_NAMETYPE_host_name            WOLFSSL_SNI_HOST_NAME

#define SSL_set_tlsext_host_name wolfSSL_set_tlsext_host_name
#define SSL_get_servername wolfSSL_get_servername
#define SSL_set_SSL_CTX                  wolfSSL_set_SSL_CTX
#define SSL_CTX_get_verify_callback      wolfSSL_CTX_get_verify_callback
#define SSL_CTX_set_tlsext_servername_callback wolfSSL_CTX_set_tlsext_servername_callback
#define SSL_CTX_set_tlsext_servername_arg      wolfSSL_CTX_set_servername_arg

#define PSK_MAX_PSK_LEN                      256
#define PSK_MAX_IDENTITY_LEN                 128
#define ERR_remove_thread_state WOLFSSL_ERR_remove_thread_state
#define SSL_CTX_clear_options wolfSSL_CTX_clear_options


#endif /* HAVE_STUNNEL || WOLFSSL_NGINX */
#define SSL_CTX_get_default_passwd_cb          wolfSSL_CTX_get_default_passwd_cb
#define SSL_CTX_get_default_passwd_cb_userdata wolfSSL_CTX_get_default_passwd_cb_userdata

/* certificate extension NIDs */
#define NID_basic_constraints         133
#define NID_key_usage                 129  /* 2.5.29.15 */
#define NID_ext_key_usage             151  /* 2.5.29.37 */
#define NID_subject_key_identifier    128
#define NID_authority_key_identifier  149
#define NID_private_key_usage_period  130  /* 2.5.29.16 */
#define NID_subject_alt_name          131
#define NID_issuer_alt_name           132
#define NID_info_access               69
#define NID_sinfo_access              79  /* id-pe 11 */
#define NID_name_constraints          144 /* 2.5.29.30 */
#define NID_certificate_policies      146
#define NID_policy_mappings           147
#define NID_policy_constraints        150
#define NID_inhibit_any_policy        168 /* 2.5.29.54 */
#define NID_tlsfeature                92  /* id-pe 24 */


#define SSL_CTX_set_msg_callback        wolfSSL_CTX_set_msg_callback
#define SSL_set_msg_callback            wolfSSL_set_msg_callback
#define SSL_CTX_set_msg_callback_arg    wolfSSL_CTX_set_msg_callback_arg
#define SSL_set_msg_callback_arg        wolfSSL_set_msg_callback_arg

/* certificate extension NIDs */
#define NID_basic_constraints         133
#define NID_key_usage                 129  /* 2.5.29.15 */
#define NID_ext_key_usage             151  /* 2.5.29.37 */
#define NID_subject_key_identifier    128
#define NID_authority_key_identifier  149
#define NID_private_key_usage_period  130  /* 2.5.29.16 */
#define NID_subject_alt_name          131
#define NID_issuer_alt_name           132
#define NID_info_access               69
#define NID_sinfo_access              79  /* id-pe 11 */
#define NID_name_constraints          144 /* 2.5.29.30 */
#define NID_certificate_policies      146
#define NID_policy_mappings           147
#define NID_policy_constraints        150
#define NID_inhibit_any_policy        168 /* 2.5.29.54 */
#define NID_tlsfeature                92  /* id-pe 24 */


#if defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY) || \
    defined(WOLFSSL_MYSQL_COMPATIBLE)

#include <wolfssl/error-ssl.h>

#define OPENSSL_STRING    WOLFSSL_STRING

#define TLSEXT_TYPE_application_layer_protocol_negotiation    16

#define OPENSSL_NPN_UNSUPPORTED 0
#define OPENSSL_NPN_NEGOTIATED  1
#define OPENSSL_NPN_NO_OVERLAP  2

/* Nginx checks these to see if the error was a handshake error. */
#define SSL_R_BAD_CHANGE_CIPHER_SPEC               LENGTH_ERROR
#define SSL_R_BLOCK_CIPHER_PAD_IS_WRONG            BUFFER_E
#define SSL_R_DIGEST_CHECK_FAILED                  VERIFY_MAC_ERROR
#define SSL_R_ERROR_IN_RECEIVED_CIPHER_LIST        SUITES_ERROR
#define SSL_R_EXCESSIVE_MESSAGE_SIZE               BUFFER_ERROR
#define SSL_R_LENGTH_MISMATCH                      LENGTH_ERROR
#define SSL_R_NO_CIPHERS_SPECIFIED                 SUITES_ERROR
#define SSL_R_NO_COMPRESSION_SPECIFIED             COMPRESSION_ERROR
#define SSL_R_NO_SHARED_CIPHER                     MATCH_SUITE_ERROR
#define SSL_R_RECORD_LENGTH_MISMATCH               HANDSHAKE_SIZE_ERROR
#define SSL_R_UNEXPECTED_MESSAGE                   OUT_OF_ORDER_E
#define SSL_R_UNEXPECTED_RECORD                    SANITY_MSG_E
#define SSL_R_UNKNOWN_ALERT_TYPE                   BUFFER_ERROR
#define SSL_R_UNKNOWN_PROTOCOL                     VERSION_ERROR
#define SSL_R_WRONG_VERSION_NUMBER                 VERSION_ERROR
#define SSL_R_DECRYPTION_FAILED_OR_BAD_RECORD_MAC  ENCRYPT_ERROR

/* Nginx uses this to determine if reached end of certs in file.
 * PEM_read_bio_X509 is called and the return error is lost.
 * The error that needs to be detected is: SSL_NO_PEM_HEADER.
 */
#define ERR_GET_LIB(l)  (int)((((unsigned long)l)>>24L)&0xffL)
#define PEM_R_NO_START_LINE     108
#define ERR_LIB_PEM             9

#ifdef HAVE_SESSION_TICKET
#define SSL_OP_NO_TICKET                  SSL_OP_NO_TICKET
#define SSL_CTRL_SET_TLSEXT_TICKET_KEY_CB 72
#endif

#define OPENSSL_config	                  wolfSSL_OPENSSL_config
#define X509_get_ex_new_index             wolfSSL_X509_get_ex_new_index
#define X509_get_ex_data                  wolfSSL_X509_get_ex_data
#define X509_set_ex_data                  wolfSSL_X509_set_ex_data
#define X509_NAME_digest                  wolfSSL_X509_NAME_digest
#define SSL_CTX_get_timeout               wolfSSL_SSL_CTX_get_timeout
#define SSL_CTX_set_tmp_ecdh              wolfSSL_SSL_CTX_set_tmp_ecdh
#define SSL_CTX_remove_session            wolfSSL_SSL_CTX_remove_session
#define SSL_get_rbio                      wolfSSL_SSL_get_rbio
#define SSL_get_wbio                      wolfSSL_SSL_get_wbio
#define SSL_do_handshake                  wolfSSL_SSL_do_handshake
#define SSL_in_init                       wolfSSL_SSL_in_init
#define SSL_get0_session                  wolfSSL_SSL_get0_session
#define X509_check_host                   wolfSSL_X509_check_host
#define i2a_ASN1_INTEGER                  wolfSSL_i2a_ASN1_INTEGER
#define ERR_peek_error_line_data          wolfSSL_ERR_peek_error_line_data
#define SSL_CTX_set_tlsext_ticket_key_cb  wolfSSL_CTX_set_tlsext_ticket_key_cb
#define X509_email_free                   wolfSSL_X509_email_free
#define X509_get1_ocsp                    wolfSSL_X509_get1_ocsp
#define SSL_CTX_set_tlsext_status_cb      wolfSSL_CTX_set_tlsext_status_cb
#define X509_check_issued                 wolfSSL_X509_check_issued
#define X509_dup                          wolfSSL_X509_dup
#define X509_STORE_CTX_new                wolfSSL_X509_STORE_CTX_new
#define X509_STORE_CTX_free               wolfSSL_X509_STORE_CTX_free
#define SSL_CTX_get_extra_chain_certs     wolfSSL_CTX_get_extra_chain_certs
#define X509_STORE_CTX_get1_issuer        wolfSSL_X509_STORE_CTX_get1_issuer
#define sk_OPENSSL_STRING_value           wolfSSL_sk_WOLFSSL_STRING_value
#define SSL_get0_alpn_selected            wolfSSL_get0_alpn_selected
#define SSL_select_next_proto             wolfSSL_select_next_proto
#define SSL_CTX_set_alpn_select_cb        wolfSSL_CTX_set_alpn_select_cb
#define SSL_CTX_set_next_protos_advertised_cb wolfSSL_CTX_set_next_protos_advertised_cb
#define SSL_CTX_set_next_proto_select_cb  wolfSSL_CTX_set_next_proto_select_cb
#define SSL_get0_next_proto_negotiated    wolfSSL_get0_next_proto_negotiated
#define SSL_is_server                     wolfSSL_is_server
#define SSL_CTX_set1_curves_list          wolfSSL_CTX_set1_curves_list

#endif

#ifdef __cplusplus
    } /* extern "C" */
#endif


#endif /* wolfSSL_openssl_h__ */
