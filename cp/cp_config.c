/*
 * Copyright (c) 2019 Sprint
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <rte_common.h>
#include <rte_debug.h>
#include <rte_eal.h>
#include <rte_cfgfile.h>

#include "cp_config.h"


#define GLOBAL_ENTRIES			"GLOBAL"
#define APN_ENTRIES				"APN_CONFIG"
#define NAMESERVER_ENTRIES		"NAMESERVER_CONFIG"
#define IP_POOL_ENTRIES			"IP_POOL_CONFIG"
#define CACHE_ENTRIES			"CACHE"
#define APP_ENTRIES				"APP"
#define OPS_ENTRIES				"OPS"

#define CP_TYPE					"CP_TYPE"
#define S11_IPS					"S11_IP"
#define S11_PORTS				"S11_PORT"
#define S5S8_IPS				"S5S8_IP"
#define S5S8_PORTS				"S5S8_PORT"
#define PFCP_IPS				"PFCP_IP"
#define PFCP_PORTS				"PFCP_PORT"
#define MME_S11_IPS				"MME_S11_IP"
#define MME_S11_PORTS			"MME_S11_PORT"
#define UPF_PFCP_IPS			"UPF_PFCP_IP"
#define UPF_PFCP_PORTS			"UPF_PFCP_PORT"
#define APN						"APN"
#define NAMESERVER				"nameserver"
#define IP_POOL_IP				"IP_POOL_IP"
#define IP_POOL_MASK			"IP_POOL_MASK"
#define CONCURRENT				"concurrent"
#define PERCENTAGE				"percentage"
#define INT_SEC					"interval_seconds"
#define FREQ_SEC				"frequency_seconds"
#define FILENAME				"filename"

/* Restoration Parameters */
#define TRANSMIT_TIMER			"TRANSMIT_TIMER"
#define PERIODIC_TIMER			"PERIODIC_TIMER"
#define TRANSMIT_COUNT			"TRANSMIT_COUNT"

int s11logger;
int s5s8logger;
int sxlogger;
int apilogger;
int epclogger;

void
config_cp_ip_port(pfcp_config_t *pfcp_config)
{
	uint32_t i = 0;
	uint32_t num_ops_entries = 0;
	uint32_t num_app_entries = 0;
	uint32_t num_cache_entries = 0;
	uint32_t num_ip_pool_entries = 0;
	uint32_t num_apn_entries = 0;
	uint32_t num_global_entries = 0;

	struct rte_cfgfile_entry *global_entries = NULL;
	struct rte_cfgfile_entry *apn_entries = NULL;
	struct rte_cfgfile_entry *ip_pool_entries = NULL;
	struct rte_cfgfile_entry *cache_entries = NULL;
	struct rte_cfgfile_entry *app_entries = NULL;
	struct rte_cfgfile_entry *ops_entries = NULL;


	struct rte_cfgfile *file = rte_cfgfile_load(STATIC_CP_FILE, 0);
	if (file == NULL) {
		rte_exit(EXIT_FAILURE, "Cannot load configuration file %s\n",
				STATIC_CP_FILE);
	}

	fprintf(stderr, "CP: PFCP Config Parsing %s\n", STATIC_CP_FILE);

	/* Read GLOBAL seaction values and configure respective params. */
	num_global_entries = rte_cfgfile_section_num_entries(file, GLOBAL_ENTRIES);

	if (num_global_entries > 0) {
		global_entries = rte_malloc_socket(NULL,
				sizeof(struct rte_cfgfile_entry) *
				num_global_entries,
				RTE_CACHE_LINE_SIZE, rte_socket_id());
	}

	if (global_entries == NULL) {
		rte_panic("Error configuring global entry of %s\n",
				STATIC_CP_FILE);
	}

	rte_cfgfile_section_entries(file, GLOBAL_ENTRIES, global_entries,
			num_global_entries);

	for (i = 0; i < num_global_entries; ++i) {

		/* Parse SGWC, PGWC and SAEGWC values from cp.cfg */
		if(strncmp(CP_TYPE, global_entries[i].name, strlen(CP_TYPE)) == 0) {
			pfcp_config->cp_type = (uint8_t)atoi(global_entries[i].value);

			fprintf(stderr, "CP: CP_TYPE     : %s\n",
					pfcp_config->cp_type == SGWC ? "SGW-C" :
					pfcp_config->cp_type == PGWC ? "PGW-C" :
					pfcp_config->cp_type == SAEGWC ? "SAEGW-C" : "UNKNOWN");

		}else if (strncmp(S11_IPS, global_entries[i].name,
					strlen(S11_IPS)) == 0) {

			inet_aton(global_entries[i].value,
					&(pfcp_config->s11_ip));

			fprintf(stderr, "CP: S11_IP      : %s\n",
					inet_ntoa(pfcp_config->s11_ip));

		}else if (strncmp(S11_PORTS, global_entries[i].name,
					strlen(S11_PORTS)) == 0) {

			pfcp_config->s11_port =
					(uint16_t)atoi(global_entries[i].value);

			fprintf(stderr, "CP: S11_PORT    : %d\n",
					pfcp_config->s11_port);

		} else if (strncmp(S5S8_IPS, global_entries[i].name,
					strlen(S5S8_IPS)) == 0) {

			inet_aton(global_entries[i].value,
					&(pfcp_config->s5s8_ip));

			fprintf(stderr, "CP: S5S8_IP     : %s\n",
					inet_ntoa(pfcp_config->s5s8_ip));

		} else if (strncmp(S5S8_PORTS, global_entries[i].name,
					strlen(S5S8_PORTS)) == 0) {

			pfcp_config->s5s8_port =
				(uint16_t)atoi(global_entries[i].value);

			fprintf(stderr, "CP: S5S8_PORT   : %d\n",
					pfcp_config->s5s8_port);

		} else if (strncmp(PFCP_IPS , global_entries[i].name,
					strlen(PFCP_IPS)) == 0) {

			inet_aton(global_entries[i].value,
					&(pfcp_config->pfcp_ip));

			fprintf(stderr, "CP: PFCP_IP     : %s\n",
					inet_ntoa(pfcp_config->pfcp_ip));

		} else if (strncmp(PFCP_PORTS, global_entries[i].name,
					strlen(PFCP_PORTS)) == 0) {

			pfcp_config->pfcp_port =
				(uint16_t)atoi(global_entries[i].value);

			fprintf(stderr, "CP: PFCP_PORT   : %d\n",
					pfcp_config->pfcp_port);

		} else if (strncmp(MME_S11_IPS, global_entries[i].name,
					strlen(MME_S11_IPS)) == 0) {

			inet_aton(global_entries[i].value,
					&(pfcp_config->s11_mme_ip));

			fprintf(stderr, "CP: MME_S11_IP  : %s\n",
					inet_ntoa(pfcp_config->s11_mme_ip));

		} else if (strncmp(MME_S11_PORTS, global_entries[i].name,
					strlen(MME_S11_PORTS)) == 0) {
			pfcp_config->s11_mme_port =
				(uint16_t)atoi(global_entries[i].value);

			fprintf(stderr, "CP: MME_S11_PORT: %d\n", pfcp_config->s11_mme_port);

		} else if (strncmp(UPF_PFCP_IPS , global_entries[i].name,
					strlen(UPF_PFCP_IPS)) == 0) {

			inet_aton(global_entries[i].value,
					&(pfcp_config->upf_pfcp_ip));

			fprintf(stderr, "CP: UPF_PFCP_IP : %s\n",
					inet_ntoa(pfcp_config->upf_pfcp_ip));

		} else if (strncmp(UPF_PFCP_PORTS, global_entries[i].name,
					strlen(UPF_PFCP_PORTS)) == 0) {

			pfcp_config->upf_pfcp_port =
				(uint16_t)atoi(global_entries[i].value);

			fprintf(stderr, "CP: UPF_PFCP_PORT: %d\n",
					pfcp_config->upf_pfcp_port);
		}

		/* Parse timer and counter values from cp.cfg */
		if(strncmp(TRANSMIT_TIMER, global_entries[i].name, strlen(TRANSMIT_TIMER)) == 0)
			pfcp_config->transmit_timer = (int)atoi(global_entries[i].value);

		if(strncmp(PERIODIC_TIMER, global_entries[i].name, strlen(PERIODIC_TIMER)) == 0)
			pfcp_config->periodic_timer = (int)atoi(global_entries[i].value);

		if(strncmp(TRANSMIT_COUNT, global_entries[i].name, strlen(TRANSMIT_COUNT)) == 0)
			pfcp_config->transmit_cnt = (uint8_t)atoi(global_entries[i].value);

	}

	rte_free(global_entries);

	/* Parse APN and nameserver values. */
	uint16_t apn_idx = 0;
	uint16_t app_nameserver_ip_idx = 0;
	uint16_t ops_nameserver_ip_idx = 0;

	num_apn_entries =
		rte_cfgfile_section_num_entries(file, APN_ENTRIES);

	if (num_apn_entries > 0) {
		/* Allocate the memory. */
		apn_entries = rte_malloc_socket(NULL,
				sizeof(struct rte_cfgfile_entry) *
				num_apn_entries,
				RTE_CACHE_LINE_SIZE, rte_socket_id());
	}

	if (apn_entries == NULL)
		rte_panic("Error configuring"
				"apn entry of %s\n", STATIC_CP_FILE);

	/* Fill the entries in APN list. */
	rte_cfgfile_section_entries(file,
			APN_ENTRIES, apn_entries, num_apn_entries);

	for (i = 0; i < num_apn_entries; ++i) {
		fprintf(stderr, "CP: [%s] = %s\n",
				apn_entries[i].name,
				apn_entries[i].value);

		if (strncmp(APN, apn_entries[i].name,
					strlen(APN)) == 0) {
			/* If key matches */
			if (i < MAX_NUM_APN) {
				char *ptr[3];
				/* Based on default value, set usage type */
				apn_list[i].apn_usage_type = -1;

				parse_apn_args(apn_entries[i].value, ptr);

				apn_list[i].apn_name_label = ptr[0];

			if (ptr[1] != NULL)
				apn_list[i].apn_usage_type = atoi(ptr[1]);

		if (ptr[2] != NULL)
			memcpy(apn_list[i].apn_net_cap, ptr[2], strlen(ptr[2]));

			set_apn_name(&apn_list[i], apn_list[i].apn_name_label);

				int f = 0;
				/* Free the memory allocated by malloc. */

				for (f = 0; f < 3; f++)
					free(ptr[f]);

				apn_idx++;
			}
		}
	}
	rte_free(apn_entries);


	/* Read cache values from cfg seaction. */
	num_cache_entries =
		rte_cfgfile_section_num_entries(file, CACHE_ENTRIES);

	if (num_cache_entries > 0) {
		cache_entries = rte_malloc_socket(NULL,
						sizeof(struct rte_cfgfile_entry)
							*num_cache_entries,
							RTE_CACHE_LINE_SIZE,
							rte_socket_id());
	}

	if (cache_entries == NULL)
		rte_panic("Error configuring"
				"CACHE entry of %s\n", STATIC_CP_FILE);

	rte_cfgfile_section_entries(file, CACHE_ENTRIES,
					cache_entries,
					num_cache_entries);

	for (i = 0; i < num_cache_entries; ++i) {
		fprintf(stderr, "CP: [%s] = %s\n",
				cache_entries[i].name,
				cache_entries[i].value);
		if (strncmp(CONCURRENT, cache_entries[i].name,
						strlen(CONCURRENT)) == 0)
			pfcp_config->dns_cache.concurrent =
					(uint32_t)atoi(cache_entries[i].value);
		if (strncmp(PERCENTAGE, cache_entries[i].name,
						strlen(CONCURRENT)) == 0)
			pfcp_config->dns_cache.percent =
					(uint32_t)atoi(cache_entries[i].value);
		if (strncmp(INT_SEC, cache_entries[i].name,
						strlen(CONCURRENT)) == 0)
			pfcp_config->dns_cache.sec =
					(uint32_t)atoi(cache_entries[i].value);
	}

	rte_free(cache_entries);

	/* Read app values from cfg seaction. */
	num_app_entries =
		rte_cfgfile_section_num_entries(file, APP_ENTRIES);

	if (num_app_entries > 0) {
		app_entries = rte_malloc_socket(NULL,
						sizeof(struct rte_cfgfile_entry)
							*num_app_entries,
							RTE_CACHE_LINE_SIZE,
							rte_socket_id());
	}

	if (app_entries == NULL)
		rte_panic("Error configuring"
				"APP entry of %s\n", STATIC_CP_FILE);

	rte_cfgfile_section_entries(file, APP_ENTRIES,
					app_entries,
					num_app_entries);

	for (i = 0; i < num_app_entries; ++i) {
		fprintf(stderr, "CP: [%s] = %s\n",
				app_entries[i].name,
				app_entries[i].value);

		if (strncmp(FREQ_SEC, app_entries[i].name,
						strlen(FREQ_SEC)) == 0)
			pfcp_config->app_dns.freq_sec =
					(uint8_t)atoi(app_entries[i].value);

		if (strncmp(FILENAME, app_entries[i].name,
						strlen(FILENAME)) == 0)
			strncpy(pfcp_config->app_dns.filename,
					app_entries[i].value,
					strlen(app_entries[i].value));

		if (strncmp(NAMESERVER, app_entries[i].name,
						strlen(NAMESERVER)) == 0) {
			strncpy(pfcp_config->app_dns.nameserver_ip[app_nameserver_ip_idx],
					app_entries[i].value,
					strlen(app_entries[i].value));
			app_nameserver_ip_idx++;
		}
	}

	pfcp_config->app_dns.nameserver_cnt = app_nameserver_ip_idx;

	rte_free(app_entries);

	/* Read ops values from cfg seaction. */
	num_ops_entries =
		rte_cfgfile_section_num_entries(file, OPS_ENTRIES);

	if (num_ops_entries > 0) {
		ops_entries = rte_malloc_socket(NULL,
						sizeof(struct rte_cfgfile_entry)
							*num_ops_entries,
							RTE_CACHE_LINE_SIZE,
							rte_socket_id());
	}

	if (ops_entries == NULL)
		rte_panic("Error configuring"
				"OPS entry of %s\n", STATIC_CP_FILE);

	rte_cfgfile_section_entries(file, OPS_ENTRIES,
					ops_entries,
					num_ops_entries);

	for (i = 0; i < num_ops_entries; ++i) {
		fprintf(stderr, "CP: [%s] = %s\n",
				ops_entries[i].name,
				ops_entries[i].value);

		if (strncmp(FREQ_SEC, app_entries[i].name,
						strlen(FREQ_SEC)) == 0)
			pfcp_config->ops_dns.freq_sec =
					(uint8_t)atoi(ops_entries[i].value);

		if (strncmp(FILENAME, ops_entries[i].name,
						strlen(FILENAME)) == 0)
			strncpy(pfcp_config->ops_dns.filename,
					ops_entries[i].value,
					strlen(ops_entries[i].value));

		if (strncmp(NAMESERVER, ops_entries[i].name,
						strlen(NAMESERVER)) == 0) {
			strncpy(pfcp_config->ops_dns.nameserver_ip[ops_nameserver_ip_idx],
					app_entries[i].value,
					strlen(ops_entries[i].value));
			ops_nameserver_ip_idx++;
		}
	}

	pfcp_config->ops_dns.nameserver_cnt = ops_nameserver_ip_idx;

	rte_free(ops_entries);

	/* Read IP_POOL_CONFIG seaction */
	num_ip_pool_entries = rte_cfgfile_section_num_entries
						(file, IP_POOL_ENTRIES);


	if (num_ip_pool_entries > 0) {
		ip_pool_entries = rte_malloc_socket(NULL,
					sizeof(struct rte_cfgfile_entry) *
					num_ip_pool_entries,
					RTE_CACHE_LINE_SIZE,
					rte_socket_id());
	}

	if (ip_pool_entries == NULL)
		rte_panic("Error configuring ip"
				"pool entry of %s\n", STATIC_CP_FILE);


	rte_cfgfile_section_entries(file, IP_POOL_ENTRIES,
					ip_pool_entries,
					num_ip_pool_entries);


	for (i = 0; i < num_ip_pool_entries; ++i) {
		fprintf(stderr, "CP: [%s] = %s\n",
				ip_pool_entries[i].name,
				ip_pool_entries[i].value);
		if (strncmp(IP_POOL_IP,
					ip_pool_entries[i].name,
					strlen(IP_POOL_IP)) == 0) {
			inet_aton(ip_pool_entries[i].value,
					&(pfcp_config->ip_pool_ip));
		} else if (strncmp
				(IP_POOL_MASK, ip_pool_entries[i].name,
				 strlen(IP_POOL_MASK)) == 0) {
			inet_aton(ip_pool_entries[i].value,
					&(pfcp_config->ip_pool_mask));
		}
	}

	rte_free(ip_pool_entries);

	return;
}

void
parse_apn_args(char *temp, char *ptr[3])
{

	int i;
	char *next = NULL, *prev = NULL;
	char *delim_ptr;

	for (i = 0; i < 3; i++) {
		ptr[i] = malloc(100);
		memset(ptr[i], 0, 100);
	}


	for (i = 0; i < 3; i++) {
		delim_ptr = strchr(temp, ',');
		next = delim_ptr;
		if (prev != NULL && next == prev + 1)
			ptr[i] = NULL;
		else if (next == NULL && *(prev + 1) == '\0')
			ptr[i] = NULL;
		else if (next == NULL)
			strcpy(ptr[i], temp);
		else
			strncpy(ptr[i], temp, delim_ptr - temp);

		if (delim_ptr != NULL)
			temp = temp + (delim_ptr - temp) + 1;
		prev = next;
	}

}

#ifdef C3PO_OSS
void
init_cli_module(pfcp_config_t *pfcp_config)
{

	clSetOption(eCLOptLogFileName, "logs/cp.log");
	clSetOption(eCLOptStatFileName, "logs/cp_stat.log");
	clSetOption(eCLOptAuditFileName, "logs/cp_sys.log");

	clInit("sgwc");

        if (spgw_cfg == SGWC || spgw_cfg == SAEGWC)
                s11logger = clAddLogger("s11");
	if (spgw_cfg == SGWC || spgw_cfg == PGWC)
		s5s8logger = clAddLogger("s5s8");
	sxlogger = clAddLogger("sx");
	apilogger = clAddLogger("api");
	epclogger = clAddLogger("epc");

	clAddRecentLogger("sgwc-001","cp",5);

	clStart();

	if (spgw_cfg == SGWC) {
		csSetName("SGWC");
		csInit(clGetStatsLogger(), get_stat_spgwc, get_stat_common, get_stat_health, get_time_stat, 2, 5000);
		csInitLastactivity(get_lastactivity_time_sgwc);
	} else if(spgw_cfg == PGWC) {
	        csSetName("PGWC");
		csInit(clGetStatsLogger(), get_stat_pgwc, get_stat_common, get_stat_health_pgwc, get_time_stat_pgwc, 2, 5000);
		csInitLastactivity(get_lastactivity_time_pgwc);
	} else {
	        csSetName("SPGWC");
		csInit(clGetStatsLogger(), get_stat_spgwc, get_stat_common, get_stat_health, get_time_stat, 2, 5000);
		csInitLastactivity(get_lastactivity_time_sgwc);
	}

	if (spgw_cfg == SGWC || spgw_cfg == SAEGWC) {

	int Interface1 = csAddInterface("S11", "gtpv2");

	//int peer1 = csAddPeer(Interface1, "false", inet_ntoa(pfcp_config->s11_mme_ip));
	int peer1 = csAddPeer(Interface1, "false", "not found");

	csAddLastactivity(Interface1, peer1, "2019-01-01T01:03:05");

	csAddHealth(Interface1, peer1, pfcp_config->transmit_timer, pfcp_config->transmit_cnt, 0);
	csAddMessage(Interface1, peer1, "Create Session Request", "in");
	csAddMessage(Interface1, peer1, "Modify Bearer Request", "in");
	csAddMessage(Interface1, peer1, "Delete Session Request", "in");


	csAddMessage(Interface1, peer1, "Number Of UEs", "in");
	csAddMessage(Interface1, peer1, "Release Access Bearers Request", "in");
	csAddMessage(Interface1, peer1, "Downlink Data Notification Acknowledge", "in");
	csAddMessage(Interface1, peer1, "Number Of PDN Connections", "in");
	csAddMessage(Interface1, peer1, "Number Of Bearers", "in");
	csAddMessage(Interface1, peer1, "Downlink Data Notification", "out");

	}

	int Interface2,peer2;

	if (spgw_cfg == SAEGWC) {
		Interface2 = csAddInterface("Sx", "PFCP");
		//peer2 = csAddPeer(Interface2, "false", inet_ntoa(pfcp_config->pfcp_ip));
		peer2 = csAddPeer(Interface2, "false", "not found");
	  } else if (spgw_cfg == SGWC) {
		Interface2 = csAddInterface("Sxa", "PFCP");
		//peer2 = csAddPeer(Interface2, "false", inet_ntoa(pfcp_config->pfcp_ip));
		peer2 = csAddPeer(Interface2, "false", "not found");
	  } else {
		Interface2 = csAddInterface("Sxb", "PFCP");
		//peer2 = csAddPeer(Interface2, "false", inet_ntoa(pfcp_config->pfcp_ip));
		peer2 = csAddPeer(Interface2, "false", "not found");
	  }

	csAddLastactivity(Interface2, peer2, "2019-01-01T01:03:05");
	csAddHealth(Interface2, peer2, pfcp_config->transmit_timer, pfcp_config->transmit_cnt, 0);
	csAddMessage(Interface2, peer2, "PFCP Session Establishment Request", "out");
	csAddMessage(Interface2, peer2, "PFCP Session Establishment Response", "in");
	csAddMessage(Interface2, peer2, "PFCP Session Deletion Request", "out");
	csAddMessage(Interface2, peer2, "PFCP Session Deletion Response", "in");
	csAddMessage(Interface2, peer2, "PFCP Association Setup Request", "out");
	csAddMessage(Interface2, peer2, "PFCP Association Setup Response", "in");

	if (spgw_cfg != PGWC)
	{
		csAddMessage(Interface2, peer2, "PFCP Session Modification Request", "out");
		csAddMessage(Interface2, peer2, "PFCP Session Modification Response", "in");
	}

	if (spgw_cfg == SGWC){
		int Interface3 = csAddInterface("S5-S8", "gtpv2");
		//int peer3 = csAddPeer(Interface3, "false", inet_ntoa(pfcp_config->s5s8_ip));
		int peer3 = csAddPeer(Interface3, "false", "not found");

		csAddLastactivity(Interface3, peer3, "2019-01-01T01:03:05");
		csAddHealth(Interface3, peer3, pfcp_config->transmit_timer, pfcp_config->transmit_cnt, 0);
		csAddMessage(Interface3, peer3, "Create Session Request", "out");
		csAddMessage(Interface3, peer3, "Create Session Response", "in");
		csAddMessage(Interface3, peer3, "Delete Session Request", "out");
		csAddMessage(Interface3, peer3, "Delete Session Response", "in");
	} else if (spgw_cfg == PGWC ){
		int Interface3 = csAddInterface("S5-S8", "gtpv2");
		//int peer3 = csAddPeer(Interface3, "false", inet_ntoa(pfcp_config->s5s8_ip));
		int peer3 = csAddPeer(Interface3, "false", "not found");

		csAddLastactivity(Interface3, peer3, "2019-01-01T01:03:05");
		csAddHealth(Interface3, peer3, pfcp_config->transmit_timer, pfcp_config->transmit_cnt, 0);
		csAddMessage(Interface3, peer3, "Create Session Request", "in");
		csAddMessage(Interface3, peer3, "Delete Session Request", "in");
		csAddMessage(Interface3, peer3, "Number Of UEs", "in");
	}

	csStart();

	init_rest_methods(12997, 1);
}
#endif /* C3PO_OSS */

