/**
 * Few stubs needed to link against libssh properly, because it uses
 * define to map some functions to OpenSSL ones that are then not inlined
 * and it uses some pointer to functions, which also doesn't work with
 * macro-based inlines.
 */

#include <proto/amissl.h>

const EVP_CIPHER *(EVP_aes_128_cbc)(void)
{
	return EVP_aes_128_cbc();
}


const EVP_CIPHER *(EVP_aes_192_cbc)(void)
{
	return EVP_aes_192_cbc();
}


const EVP_CIPHER *(EVP_aes_256_cbc)(void)
{
	return EVP_aes_256_cbc();
}

const EVP_CIPHER *(EVP_bf_cbc)(void)
{
	return EVP_bf_cbc();
}

const EVP_CIPHER *(EVP_rc4)(void)
{
	return EVP_rc4();
}

const EVP_CIPHER *(EVP_cast5_cbc)(void)
{
	return EVP_cast5_cbc();
}

const EVP_CIPHER *(EVP_des_ede3_cbc)(void)
{
	return EVP_des_ede3_cbc();
}

RSA *(PEM_read_bio_RSAPrivateKey)(BIO *bp, RSA **x, pem_password_cb *cb, void *u)
{
	return PEM_read_bio_RSAPrivateKey(bp, x, cb, u);
}


DSA *(PEM_read_bio_DSAPrivateKey)(BIO *bp, DSA **x, pem_password_cb *cb, void *u)
{
	return PEM_read_bio_DSAPrivateKey(bp, x, cb, u);
}
