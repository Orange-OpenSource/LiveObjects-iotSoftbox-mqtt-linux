/*
 * Copyright (C) 2016 Orange
 *
 * This software is distributed under the terms and conditions of the 'BSD-3-Clause'
 * license which can be found in the file 'LICENSE.txt' in this package distribution
 * or at 'https://opensource.org/licenses/BSD-3-Clause'.
 *
 * This file is a part of LiveObjects iotsoftbox-mqtt library.
 */

/**
 * @file  liveobjects_dev_security.h
 *
 * @brief Define certificates and TLS mode
 *
 */
#ifndef __liveobjects_dev_security_H_
#define __liveobjects_dev_security_H_

#define VERIFY_MODE   0

/*#define SERVER_CERT          NULL*/
#define CLIENT_CERT           NULL
#define CLIENT_PKEY           NULL
#define CLIENT_PKEY_PASSWORD  0

#define SERVER_CERTIFICATE_COMMON_NAME     "m2m.orange.com"

#ifndef SERVER_CERT

#define SERVER_CERT  "-----BEGIN CERTIFICATE-----\n" \
        "MIICkzCCAfygAwIBAgIBFzANBgkqhkiG9w0BAQUFADBdMQswCQYDVQQGEwJGUjEN\n" \
        "MAsGA1UECAwETHlvbjEPMA0GA1UECgwGT3JhbmdlMQwwCgYDVQQLDANPQUIxIDAe\n" \
        "BgNVBAMMF0dyb3VwZSBPcmFuZ2UgSWFlcyBDQSAxMB4XDTE1MTEyMzA5MjAxMFoX\n" \
        "DTM1MTExODA5MjAxMFowXTELMAkGA1UEBhMCRlIxDTALBgNVBAgMBEx5b24xDzAN\n" \
        "BgNVBAoMBk9yYW5nZTEMMAoGA1UECwwDT0FCMSAwHgYDVQQDDBdHcm91cGUgT3Jh\n" \
        "bmdlIElhZXMgQ0EgMTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAlsdFGeMj\n" \
        "4b4jS4JNscoplDWR9WaZX8lYHOjp0axIEQXTusOBvk22YofY5Tnv0x78S+wRuOH5\n" \
        "qgLPXClpshg8WtX2iwcvH0VxYfHIutZ1Whl4delpC5oiMs/Sa5h5VoBxEYu9EpCg\n" \
        "7EZc292xFoVFTQY/wUAgPKFbUdthDz9J/JECAwEAAaNjMGEwHQYDVR0OBBYEFFyA\n" \
        "eJlmSmyL24lhK5/fDoS6tdv5MB8GA1UdIwQYMBaAFFyAeJlmSmyL24lhK5/fDoS6\n" \
        "tdv5MBIGA1UdEwEB/wQIMAYBAf8CAQAwCwYDVR0PBAQDAgEGMA0GCSqGSIb3DQEB\n" \
        "BQUAA4GBADvFP7F3vfpX6MP24XO4OE7JK5qa0CjC2Mt45YgzH9QtaHruSNPuoNJD\n" \
        "wspRAvBRkq2kJAl3KGuC3zSfP4VTCa4PX1w+pPU5OpSWxfgt1LauP/BRcxcrEqmF\n" \
        "hvj7auQ4LM2NkuLqxqernaKnocYMiwbgq/PlUtOYd6gZtehj6uSw\n" \
        "-----END CERTIFICATE-----\n"
#endif

#ifndef CLIENT_CERT
#define CLIENT_CERT "-----BEGIN CERTIFICATE-----\n" \
        "-----END CERTIFICATE-----\n"
#endif

#ifndef CLIENT_PKEY

#if CLIENT_PKEY_PASSWORD
const char* getLomPkeyPassword(void);
#endif

#define CLIENT_PKEY[] "-----BEGIN PRIVATE KEY-----\n" \
        "-----END PRIVATE KEY-----\n"

#endif /* CLIENT_PKEY*/

#endif /* __liveobjects_dev_security_H_*/
