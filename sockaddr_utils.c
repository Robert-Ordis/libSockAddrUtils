#include "include/sockaddr_utils/sockaddr_utils.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>

#include <string.h>
#include <stdlib.h>

const static uint8_t	bin_addr_map_head[12] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF};
const static struct sockaddr_un sample_un;
const static size_t un_path_len = sizeof(sample_un.sun_path) / sizeof(sample_un.sun_path[0]);

//uint8_t[4](BE)からIPv4アドレス情報をセットする
static int	sockaddr_set_v4_addr_(struct sockaddr_in *saddr_v4, uint8_t *bin_addr){
	
	//int i;
	
	saddr_v4->sin_family = AF_INET;
	//saddr_v4->sin_addr.s_addr = 0;
	//memcpy(&saddr_v4->sin_addr.s_addr, bin_addr, 4);
	saddr_v4->sin_addr.s_addr = (*((uint32_t *)bin_addr));
	return 0;
}

//uint8_t[16](BE)からIPv6アドレス情報をセットする
static int	sockaddr_set_v6_addr_(struct sockaddr_in6 *saddr_v6, uint8_t *bin_addr){
	
	//int i;
	
	saddr_v6->sin6_family = AF_INET6;
	/*
	memset(&(saddr_v6->sin6_addr), 0, sizeof(struct in6_addr));
	*/
	/*
	for(i = 0; i < 16; i++){
		saddr_v6->sin6_addr.s6_addr[i] = bin_addr[i];
	}
	*/
	memcpy(&(saddr_v6->sin6_addr.s6_addr[0]), bin_addr, sizeof(char) * 16);
	return 0;
}

//uint16_t(LE)から、ポート番号をセットする＠IPv4
static int	sockaddr_set_v4_port_(struct sockaddr_in *saddr_v4, uint16_t port){
	
	saddr_v4->sin_port = htons(port);
	return 0;
}

//uint16_t(LE)から、ポート番号をセットする＠IPv6
static int	sockaddr_set_v6_port_(struct sockaddr_in6 *saddr_v6, uint16_t port){
	
	saddr_v6->sin6_port = htons(port);
	return 0;
}

//ホスト情報文字列から、IPアドレスをセットする（v4/v6共通）
static int	sockaddr_set_host_(struct sockaddr *saddr, char *host, struct addrinfo *hints){
	struct addrinfo	*res_addrs;
	int				ret;
	
	if((ret = getaddrinfo(host, NULL, hints, &res_addrs)) != 0){
		errno = ret;
		return -1;
	}
	
	ret = 0;
	uint16_t ex_port = sockaddr_get_portno(saddr);
	//struct sockaddr *saddr_res = res_addrs->ai_addr;
	switch(res_addrs->ai_family){
	case AF_INET:
		memcpy(saddr, res_addrs->ai_addr, sizeof(struct sockaddr_in));
		break;
	case AF_INET6:
		memcpy(saddr, res_addrs->ai_addr, sizeof(struct sockaddr_in6));
		break;
	default:
		ret = -1;
		break;
	}
	
	sockaddr_set_portno(saddr, ex_port);
	freeaddrinfo(res_addrs);
	return ret;
}


//全アクセス情報のセット。bin_addrはバイナリ表現のアドレス。（よって、長さも固定）
int			sockaddr_set_v4_info(struct sockaddr *saddr, uint8_t *bin_addr, uint16_t port){
	int ret;
	if((ret = sockaddr_set_v4_addr_((struct sockaddr_in *)saddr, bin_addr)) != 0){
		return ret;
	}
	if((ret = sockaddr_set_v4_port_((struct sockaddr_in *)saddr, port)) != 0){
		return ret;
	}
	return ret;
}
int			sockaddr_set_v6_info(struct sockaddr *saddr, uint8_t *bin_addr, uint16_t port){
	int ret;
	if((ret = sockaddr_set_v6_addr_((struct sockaddr_in6 *)saddr, bin_addr)) != 0){
		return ret;
	}
	if((ret = sockaddr_set_v6_port_((struct sockaddr_in6 *)saddr, port)) != 0){
		return ret;
	}
	return ret;
}
int			sockaddr_set_un_info(struct sockaddr *saddr, char *path, size_t path_len){
	//int ret;
	int len;
	
	len = strlen(path) + 1;	//文字列長+'\0'
	
	if(len > un_path_len){
		//unixソケット構造体に収まらないためNG
		errno = ENOSPC;
		return -1;
	}
	else if(len == 0){
		//path[0]＝抽象ソケット
		len = un_path_len;
	}
	
	struct sockaddr_un *saddr_un = (struct sockaddr_un*) saddr;
	memset(saddr_un, 0, sizeof(struct sockaddr_un));
	saddr_un->sun_family = AF_UNIX;
	memcpy(&(saddr_un->sun_path[0]), path, len);
	
	return 0;
}

//(INET)アドレスだけ。単純なバイナリ表現から落とし込む。
int			sockaddr_set_v4_addr(struct sockaddr *saddr, uint8_t *src){
	return sockaddr_set_v4_addr_((struct sockaddr_in *) saddr, src);
}
int			sockaddr_set_v6_addr(struct sockaddr *saddr, uint8_t *src){
	return sockaddr_set_v6_addr_((struct sockaddr_in6 *) saddr, src);
}

size_t		sockaddr_get_inet_addr(struct sockaddr *saddr, uint8_t *dst, size_t dst_len){
	int i;
	switch(saddr->sa_family){
	case AF_INET:
		if(dst_len < 4){
			return -1;
		}
		for(i = 0; i < 4; i++){
			dst[i] = (uint8_t)(((((struct sockaddr_in *)saddr)->sin_addr.s_addr) >> (8 * i)) & 0x000000FF);
		}
		return 4;
	case AF_INET6:
		if(dst_len < 16){
			return -1;
		}
		memcpy(dst, &(((struct sockaddr_in6 *)saddr)->sin6_addr.s6_addr[0]), 16);
		return 16;
	default:
		errno = EAFNOSUPPORT;
		return 0;
	}
}


//(INET)文字列から。名前解決コミみたいなならこちら
int			sockaddr_set_v4_host(struct sockaddr *saddr, char *host){
	struct addrinfo	hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	//hints.ai_socktype = SOCK_UNSPEC;
	
	return sockaddr_set_host_(saddr, host, &hints);
}
int			sockaddr_set_v6_host(struct sockaddr *saddr, char *host){
	struct addrinfo	hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET6;
	//hints.ai_socktype = SOCK_UNSPEC;
	
	return sockaddr_set_host_(saddr, host, &hints);
}
int			sockaddr_set_any_host(struct sockaddr *saddr, char *host){
	struct addrinfo	hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	//hints.ai_socktype = SOCK_UNSPEC;
	
	return sockaddr_set_host_(saddr, host, &hints);
}

ssize_t		sockaddr_get_dst_str(struct sockaddr *saddr, char *dst_host, size_t dst_len){
	ssize_t size;
	char *tmp;
	memset(dst_host, 0, dst_len);
	switch(saddr->sa_family){
	case AF_UNIX:
		//パス名/抽象パス
		tmp = ((struct sockaddr_un*)saddr)->sun_path;
		if(tmp[0] == '\0'){
			size = un_path_len;
		}
		else{
			size = (ssize_t)strlen(tmp);
		}
		
		if(size > dst_len){
			errno = ENOSPC;
			return -1;
		}
		
		memcpy(dst_host, tmp, size);
		return size;
	case AF_INET:
		tmp = (char*)&(((struct sockaddr_in *)saddr)->sin_addr);
		break;
	case AF_INET6:
		tmp = (char*)&(((struct sockaddr_in6 *)saddr)->sin6_addr);
		break;
	default:
		errno = EAFNOSUPPORT;
		return -1;
	}
	if(inet_ntop(saddr->sa_family, (void*)tmp, dst_host, dst_len) == NULL){
		return (ssize_t)-1;
	}
	return (ssize_t)strlen(dst_host);
}

//ポート周り
int			sockaddr_set_portno(struct sockaddr *saddr, uint16_t port){
	switch(saddr->sa_family){
	case AF_INET:
		return sockaddr_set_v4_port_((struct sockaddr_in*) saddr, port);
	case AF_INET6:
		return sockaddr_set_v6_port_((struct sockaddr_in6*) saddr, port);
	default:
		errno = EAFNOSUPPORT;
		return -1;
	}
}

uint16_t	sockaddr_get_portno(struct sockaddr *saddr){
	switch(saddr->sa_family){
	case AF_INET:
		return ntohs(((struct sockaddr_in *)saddr)->sin_port);
	case AF_INET6:
		return ntohs(((struct sockaddr_in6 *)saddr)->sin6_port);
	default:
		errno = EAFNOSUPPORT;
		return 0;
	}
}

//その他
size_t		sockaddr_get_size(struct sockaddr *saddr){
	switch(saddr->sa_family){
	case AF_INET:
		return sizeof(struct sockaddr_in);
	case AF_INET6:
		return sizeof(struct sockaddr_in6);
	case AF_UNIX:
		return sizeof(struct sockaddr_un);
	default:
		return sizeof(struct sockaddr);
	}
}

//比較
int			sockaddr_compare_total(struct sockaddr *saddr_a, struct sockaddr *saddr_b){
	
	int ret;
	if((ret = sockaddr_compare_addr(saddr_a, saddr_b)) == 0){
		return sockaddr_compare_portno(saddr_a, saddr_b);
	}
	
	return ret;
}

//アドレス比較。ファミリ→
int			sockaddr_compare_addr(struct sockaddr *saddr_a, struct sockaddr *saddr_b){
	void *a;
	void *b;
	size_t len;
	if(saddr_a->sa_family != saddr_b->sa_family){
		return saddr_a->sa_family - saddr_b->sa_family;
	}
	
	switch(saddr_a->sa_family){
	case AF_UNIX:
		a = &(((struct sockaddr_un*)saddr_a)->sun_path[0]);
		b = &(((struct sockaddr_un*)saddr_b)->sun_path[0]);
		len = un_path_len;
		break;
	case AF_INET:
		a = &(((struct sockaddr_in*)saddr_a)->sin_addr.s_addr);
		b = &(((struct sockaddr_in*)saddr_b)->sin_addr.s_addr);
		len = 4;
		break;
	case AF_INET6:
		a = &(((struct sockaddr_in6*)saddr_a)->sin6_addr.s6_addr[0]);
		b = &(((struct sockaddr_in6*)saddr_b)->sin6_addr.s6_addr[0]);
		len = 16;
		break;
	default:
		errno = EAFNOSUPPORT;
		return 0;
	}
	return memcmp(a, b, len);
}
int			sockaddr_compare_portno(struct sockaddr *saddr_a, struct sockaddr *saddr_b){
	int a = (int)sockaddr_get_portno(saddr_a);
	int b = (int)sockaddr_get_portno(saddr_b);
	return a - b;
}

//射影：0:0:0:0:0:FFFF:[IPv4/4bytes]
int		sockaddr_storage_4to6(struct sockaddr_storage *saddr){
	uint8_t	bin_addr_6[16];
	
	if(sockaddr_get_inet_addr((struct sockaddr *)saddr, &bin_addr_6[12], 4) != 4){
		errno = EAFNOSUPPORT;
		return -1;
	}
	memcpy(bin_addr_6, bin_addr_map_head, 12);
	
	return sockaddr_set_v6_addr_((struct sockaddr_in6 *)saddr, bin_addr_6);
}

//射影であることをチェックし、そこからv4にする。
int		sockaddr_storage_6to4(struct sockaddr_storage *saddr){
	uint8_t	bin_addr_6[16];
	if(sockaddr_get_inet_addr((struct sockaddr *)saddr, &bin_addr_6[0], 16) != 16){
		errno = EAFNOSUPPORT;
		return -1;
	}
	
	//射影アドレスでない
	if(memcmp(bin_addr_6, bin_addr_map_head, 12) != 0){
		errno = EAFNOSUPPORT;
		return -1;
	}
	
	return sockaddr_set_v4_addr_((struct sockaddr_in *)saddr, &bin_addr_6[12]);
}

int		sockaddr_is_ipv4(struct sockaddr *saddr, int include_mapped){
	uint8_t	bin_addr_6[16];
	switch(saddr->sa_family){
	case AF_INET:
		//IPv4なのでそのままtrue
		return 1;
	case AF_INET6:
		//IPv6なので、射影かどうかを検査する
		if(include_mapped){
			if(sockaddr_get_inet_addr(saddr, bin_addr_6, 16) == 16){
				if(memcmp(bin_addr_6, bin_addr_map_head, 12) == 0){
					return 1;
				}
				return 0;
			}
		}
		return 0;
	default:
		return 0;
	}
}

