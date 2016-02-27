#ifndef __NGX_AWS_FUNCTIONS_INTERNAL__H__
#define __NGX_AWS_FUNCTIONS_INTERNAL__H__

#include <time.h>
#include <ngx_times.h>
#include <ngx_core.h>
#include <ngx_http.h>

#include "crypto_helper.h"

#define AMZ_DATE_MAX_LEN 20
#define STRING_TO_SIGN_LENGTH 3000

static const ngx_str_t EMPTY_STRING_SHA256 = ngx_string("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
static const ngx_str_t EMPTY_STRING = ngx_null_string;

static inline const ngx_str_t* ngx_aws_auth__compute_request_time(ngx_pool_t *pool, const time_t *timep) {
	ngx_str_t *const retval = ngx_palloc(pool, sizeof(ngx_str_t));
	retval->data = ngx_palloc(pool, AMZ_DATE_MAX_LEN);
	struct tm *tm_p = ngx_palloc(pool, sizeof(struct tm));
	gmtime_r(timep, tm_p);
	retval->len = strftime(retval->data, AMZ_DATE_MAX_LEN - 1, "%Y%m%dT%H%M%SZ", tm_p);
	return retval;
}

static inline const ngx_str_t* ngx_aws_auth__canonize_query_string(ngx_pool_t *pool,
	ngx_http_request_t *req) {
	/* TODO: impl */
	return &EMPTY_STRING;
}


static inline const ngx_str_t* ngx_aws_auth__host_from_bucket(ngx_pool_t *pool, ngx_str_t *s3_bucket) {
	static const char HOST_PATTERN[] = ".s3.amazonaws.com";
	ngx_str_t *host;

	host = ngx_palloc(pool, sizeof(ngx_str_t));
	host->len = s3_bucket->len + sizeof(HOST_PATTERN) + 1;
	host->data = ngx_palloc(pool, host->len);
	ngx_snprintf(host->data, host->len, "%v%s", s3_bucket, HOST_PATTERN);
	host->len = strnlen(host->data, host->len);

	return host;
}

static inline const ngx_str_t* ngx_aws_auth__canonize_headers(ngx_pool_t *pool,
	ngx_http_request_t *req,
	ngx_str_t *s3_bucket, ngx_str_t *date, ngx_str_t *content_hash) {
	static const char HOST_PATTERN[] = ".s3.amazonaws.com";

	const ngx_str_t *host = ngx_aws_auth__host_from_bucket(pool, s3_bucket);
}

static inline const ngx_str_t* ngx_aws_auth__request_body_hash(ngx_pool_t *pool,
	ngx_http_request_t *req) {
	/* TODO: support cases involving non-empty body */
	return &EMPTY_STRING_SHA256;
}

static inline const ngx_str_t* ngx_aws_auth__make_canonical_request(ngx_pool_t *pool,
	ngx_http_request_t *req,
	ngx_str_t *s3_bucket_name) {
	const ngx_str_t *canon_qs;
	const ngx_str_t *requst_body_hash;

	// canonize query string
	canon_qs = ngx_aws_auth__canonize_query_string(pool, req);

	// compute request body hash
	requst_body_hash = ngx_aws_auth__request_body_hash(pool, req);

	// canonize headers
	
	// TODO: compute retval
	return NULL;
}

static inline const ngx_str_t* ngx_aws_auth__string_to_sign(ngx_pool_t *pool,
	ngx_http_request_t *req, ngx_str_t *s3_bucket_name, ngx_str_t *key_scope) {
	const ngx_str_t *canon_request = ngx_aws_auth__make_canonical_request(pool, req, s3_bucket_name);
	const ngx_str_t *canon_request_hash = ngx_aws_auth__hash_sha256(pool, canon_request);
	const ngx_str_t *date; // TODO: set the date
	ngx_str_t *retval = ngx_palloc(pool, sizeof(ngx_str_t));

	retval->data = ngx_palloc(pool, STRING_TO_SIGN_LENGTH);
	retval->len = STRING_TO_SIGN_LENGTH;
	ngx_snprintf(retval->data, retval->len, "AWS4-HMAC-SHA256\n%v\n%v\n%v",
		date, key_scope, canon_request_hash);
	retval->len = strnlen(retval->data, retval->len);

	return retval;
}

static inline const ngx_str_t* ngx_aws_auth__make_auth_token(ngx_pool_t *pool,
	ngx_str_t *signature, ngx_str_t *signed_header_names,
	ngx_str_t *access_key_id, ngx_str_t *key_scope) {

    const char FMT_STRING[] = "AWS4-HMAC-SHA256 Credential=%v/%v,SignedHeaders=%v,Signature=%v";
	ngx_str_t *authz;

	authz = ngx_palloc(pool, sizeof(ngx_str_t));
	authz->len = access_key_id->len + key_scope->len + signed_header_names->len
		+ signature->len + sizeof(FMT_STRING);
	authz->data = ngx_palloc(pool, authz->len);
    ngx_snprintf(authz->data, authz->len, FMT_STRING,
		access_key_id, key_scope, signed_header_names, signature);
	authz->len = strnlen(authz->data, authz->len);
	return authz;
}


static inline void ngx_aws_auth__sign(ngx_pool_t *pool, ngx_http_request_t *req,
	ngx_str_t *access_key_id, ngx_str_t *secret, ngx_str_t *key_scope) {

	// TODO:
	// get string to sign
	// generate signature
	// constuct auth header
	// attach auth header to outbound response
}

#endif
