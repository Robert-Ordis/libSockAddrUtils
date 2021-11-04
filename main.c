#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sockaddr_utils/sockaddr_utils.h>

int main(int argc, char *argv[]){
	
	char *input_addr_a = NULL;
	char *input_addr_b = NULL;
	struct sockaddr_storage sa;
	struct sockaddr_storage sb;
	struct sockaddr *a = (struct sockaddr *)&sa;
	struct sockaddr *b = (struct sockaddr *)&sb;
	char tmp_chars[512];
	int opt;
	int in_unix = 0;
	int in_map_experiment = 0;
	while((opt = getopt(argc, argv, "a:b:um")) != -1){
		switch(opt){
		case 'a':
			input_addr_a = optarg;
			break;
		case 'b':
			input_addr_b = optarg;
			break;
		case 'u':
			in_unix = 1;
			break;
		case 'm':
			in_map_experiment = 1;
		}
	}
	if(input_addr_a == NULL){
		printf("input a's path\n");
		return 0;
	}
	
	if(in_unix){
		//printf("%s: in_unix, len is %d\n", __func__, un_path_len);
		if(sockaddr_set_un_info(a, input_addr_a, strlen(input_addr_a)) < 0){
			perror("sockaddr_set_un_info[a]");
			exit(-1);
		}
		printf("%s omake. Port is %d\n", __func__, sockaddr_get_portno(a));
		sockaddr_get_dst_str(a, tmp_chars, 512);
		printf("%s: a[%d] is %s\n", __func__, sa.ss_family, tmp_chars);
		printf("%s: sa_len->%zu\n", __func__, sockaddr_get_size(a));
		if(input_addr_b == NULL){
			return 0;
		}
		
		if(sockaddr_set_un_info(b, input_addr_b, strlen(input_addr_b)) < 0){
			perror("sockaddr_set_un_info[b]");
			exit(-1);
		}
		
		sockaddr_get_dst_str(b, tmp_chars, 512);
		printf("%s: b[%d] is %s\n", __func__, sb.ss_family, tmp_chars);
		
		printf("%s: then compare->%d\n", __func__, sockaddr_compare_total(a, b));
		
		return 0;
	}
	
	if(sockaddr_set_any_host(a, input_addr_a) < 0){
		perror("sockaddr_set_any_host[a]");
		exit (-1);
	}
	sockaddr_set_portno(a, 4445);
	sockaddr_get_dst_str(a, tmp_chars, 512);
	printf("%s: a[%d] is %s:%u\n", __func__, sa.ss_family, tmp_chars, sockaddr_get_portno(a));
	printf("%s: sa_len->%zu\n", __func__, sockaddr_get_size(a));
	
	if(in_map_experiment){
		printf("%s: is_v4(0) is %d\n", __func__, sockaddr_is_ipv4(a, 0));
		printf("%s: is_v4(1) is %d\n", __func__, sockaddr_is_ipv4(a, 1));
		
		if(sockaddr_storage_4to6((struct sockaddr_storage *)a) < 0){
			perror("sockaddr_storage_4to6:first");
		}
		
		if(sockaddr_storage_6to4((struct sockaddr_storage *)a) < 0){
			perror("sockaddr_storage_6to4");
			exit(1);
		}
		sockaddr_get_dst_str(a, tmp_chars, 512);
		printf("%s: a[%d] is %s:%u\n", __func__, sa.ss_family, tmp_chars, sockaddr_get_portno(a));
		printf("%s: sa_len->%zu\n", __func__, sockaddr_get_size(a));
		
		if(sockaddr_storage_4to6((struct sockaddr_storage *)a) < 0){
			perror("sockaddr_storage_4to6");
			exit(1);
		}
		sockaddr_get_dst_str(a, tmp_chars, 512);
		printf("%s: a[%d] is %s:%u\n", __func__, sa.ss_family, tmp_chars, sockaddr_get_portno(a));
		printf("%s: sa_len->%zu\n", __func__, sockaddr_get_size(a));
		
		return 0;
	}
	
	if(input_addr_b == NULL){
		return 0;
	}
	
	if(sockaddr_set_any_host(b, input_addr_b) < 0){
		perror("sockaddr_set_any_host[b]");
		exit (-1);
	}
	sockaddr_set_portno(b, 4446);
	sockaddr_get_dst_str(b, tmp_chars, 512);
	printf("%s: b[%d] is %s:%u\n", __func__, sb.ss_family, tmp_chars, sockaddr_get_portno(b));
	
	printf("%s: then, comparison\n", __func__);
	printf("%s: total compare:%d\n", __func__, sockaddr_compare_total(a, b));
	printf("%s: addr only->%d\n", __func__, sockaddr_compare_addr(a, b));
	printf("%s: port compare: %d\n", __func__, sockaddr_compare_portno(a, b));
	
	return 0;
}
