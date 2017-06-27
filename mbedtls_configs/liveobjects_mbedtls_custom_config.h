/*
 *  Minimal configuration for using mbedtls as part of mbed-client
 *
 *  Copyright (C) 2006-2016, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

/*
 * Build from mbedtls_mbed_client_config.h on web site : https://github.com/ARMmbed/mbed-os-example-client
 * Change MBEDTLS_SSL_CIPHERSUITES
 */

#ifndef MBEDTLS_CUSTOM_CONFIG_H
#define MBEDTLS_CUSTOM_CONFIG_H

// Remove selftesting and save 11KB of ROM
#undef MBEDTLS_SELF_TEST

// Reduces ROM size by 30 kB
#undef MBEDTLS_ERROR_STRERROR_DUMMY
#undef MBEDTLS_VERSION_FEATURES
#undef MBEDTLS_DEBUG_C

// define to save 8KB RAM at the expense of ROM
#undef MBEDTLS_AES_ROM_TABLES

// Save ROM and a few bytes of RAM by specifying our own ciphersuite list
#ifdef MBEDTLS_SSL_CIPHERSUITES
#undef MBEDTLS_SSL_CIPHERSUITES
#endif
#define MBEDTLS_SSL_CIPHERSUITES MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256

#endif /* MBEDTLS_CUSTOM_CONFIG_H */
