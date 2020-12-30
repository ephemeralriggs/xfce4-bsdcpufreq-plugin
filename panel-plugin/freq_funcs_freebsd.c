/* Copyright (c) 2014-2020, Thomas Zander <thomas.e.zander@googlemail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <sys/sysctl.h>
#include <sys/sysctl.h>

#include "freq_funcs.h"

#define SYSCTL_FREQ_FORMAT_STRING "dev.cpu.%d.freq"
#define SYSCTL_FREQ_LEVELS_FORMAT_STRING "dev.cpu.%d.freq_levels"

/*! \brief Returns an int value from a sysctl
 *
 * \param mib a correctly filled mib structure, see sysctl(3)
 * \param miblen the length of the mib in elements
 * \param error possible errors while calling sysctl are reported here, written
 * \return the integer return value of the sysctl
 */
int get_int_sysctl_by_mib(const int *mib, size_t miblen, int *error)
{
	int v;
	size_t lv = sizeof(v);

	*error = sysctl(mib, miblen, &v, &lv, NULL, 0);
	return v;
}

/*! \brief Returns the mib and size for a given CPU ID
 *
 * \param mib pointer to a sufficiently large mib structure, see sysctl(3), written
 * \param miblen the length of the mib in elements
 * \param cpu the CPU ID to get the frequency for, typically a small positive integer, in most cases 0
 * \param error possible errors while calling sysctl are reported here, written
 * \return the length of the mib to be used in subsequent accesses
 */
int get_mib_for_cpu_freq(int *mib, size_t miblen, int cpu, int *error)
{
	char sysctl_freq[32];
	size_t len = miblen;

	snprintf(sysctl_freq, sizeof(sysctl_freq)/sizeof(sysctl_freq[0]), SYSCTL_FREQ_FORMAT_STRING, cpu);
	*error = sysctlnametomib(sysctl_freq, mib, &len);
	return len;
}

/*! \brief Returns the maximum CPU frequency for a given CPU ID
 *
 * \param cpu the CPU ID to get the maximum level for, typically a small positive integer, in most cases 0
 * \return the maximum CPU frequency speed, in MHz
 */
int get_max_cpufreq_level(int cpu)
{
	char buf[512], sysctl_freq[32];
	size_t len = sizeof(buf)/sizeof(buf[0]);
	int r;

	snprintf(sysctl_freq, sizeof(sysctl_freq)/sizeof(sysctl_freq[0]), SYSCTL_FREQ_LEVELS_FORMAT_STRING, cpu);

	r = sysctlbyname(sysctl_freq, buf, &len, NULL, 0);
	if (r < 0) return -1;	// Data on CPU not available

	r = strtol(buf, NULL, 10);
	return r;
}
// vim:ts=8
