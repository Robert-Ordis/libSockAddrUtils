/**
 *	\file		sockaddr_utils.h
 *	\brief		私の考えるsockaddr関連の便利関数をまとめたもの
 *	\remarks	ポート単体セット以外はsa_familyの書き換えも伴う
 *	\remarks	uint8_t *bin_addrの引数は実スペース足りないとセグフォるから注意
 */

#ifndef	SOCKADDR_UTILS_H
#define	SOCKADDR_UTILS_H

#include <stdint.h>
#include <sys/socket.h>

//全アクセス情報のセット。bin_addrはバイナリ表現のアドレス。（よって、長さも固定）
/**
 *	\fn				sockaddr_set_v4_info
 *	\brief			アドレスのバイナリ表現とポートを置く＠IPv4
 *	\param [out]	saddr		ソケットアドレス。sockaddr_storage推奨？
 *	\param [in]	bin_addr	アドレスのバイナリ表現。4バイトは持ってきてね。
 *	\param [in]	port		ポート番号。普通にホストシステムのエンディアンでいいよ。
 *	\return		成功時0
 */
int			sockaddr_set_v4_info(struct sockaddr *saddr, uint8_t *bin_addr, uint16_t port);

/**
 *	\fn				sockaddr_set_v6_info
 *	\brief			アドレスのバイナリ表現とポートを置く＠IPv6
 *	\param [out]	saddr		ソケットアドレス。sockaddr_storage推奨？
 *	\param [in]	bin_addr	アドレスのバイナリ表現。16バイトは持ってきてね。
 *	\param [in]	port		ポート番号。普通にホストシステムのエンディアンでいいよ。
 *	\return		成功時0
 */
int			sockaddr_set_v6_info(struct sockaddr *saddr, uint8_t *bin_addr, uint16_t port);

/**
 *	\fn				sockaddr_set_un_info
 *	\brief			アドレスのパスを置く@Unixソケット
 *	\param [out]	saddr		ソケットアドレス。sockaddr_storage推奨？
 *	\param [in]	path		UNIXソケットのパス
 *	\param [in]	path_len	pathのバッファ長
 *	\return		成功時0。失敗時-1
 */
int			sockaddr_set_un_info(struct sockaddr *saddr, char *path, size_t path_len);
//(INET)アドレスだけ。単純なバイナリ表現から落とし込む。

/**
 *	\fn				sockaddr_set_v4_addr
 *	\brief			IPアドレスを書き込む＠IPv4
 *	\param [out]	saddr		ソケットアドレス。sockaddr_storage推奨？
 *	\param [in]	bin_addr	アドレスのバイナリ表現。4バイトは持ってきてね。
 *	\return		成功時0。失敗時-1
 *	\remarks		自動的にfamilyはAF_INETになります
 */
int			sockaddr_set_v4_addr(struct sockaddr *saddr, uint8_t *src);

/**
 *	\fn				sockaddr_set_v6_addr
 *	\brief			IPアドレスを書き込む＠IPv6
 *	\param [out]	saddr		ソケットアドレス。sockaddr_storage推奨？
 *	\param [in]	bin_addr	アドレスのバイナリ表現。16バイトは持ってきてね。
 *	\return		成功時0。失敗時-1
 *	\remarks		自動的にfamilyはAF_INET6になります
 */
int			sockaddr_set_v6_addr(struct sockaddr *saddr, uint8_t *src);

/**
 *	\fn				sockaddr_get_inet_addr
 *	\brief			IPアドレスのバイナリ表現を取得する
 *	\param [out]	saddr		ソケットアドレス。sockaddr_storage推奨？
 *	\param [in]	bin_addr	アドレスのバイナリ表現。16バイトは持ってきてね。
 *	\return		成功時0。失敗時-1
 *	\remarks		AF_INET, AF_INET6のみ。
 */
size_t		sockaddr_get_inet_addr(struct sockaddr *saddr, uint8_t *dst, size_t dst_len);

//(INET)文字列から。名前解決コミみたいなならこちら

/**
 *	\fn				sockaddr_set_v4_host
 *	\brief			ホスト名からIPv4を取得する
 *	\param [out]	saddr		ソケットアドレス。sockaddr_storage推奨？
 *	\param [in]	host		ホスト名
 *	\return		成功時0。失敗時-1
 *	\remarks		成功したらfamilyは書き換わります
 */
int			sockaddr_set_v4_host(struct sockaddr *saddr, char *host);

/**
 *	\fn				sockaddr_set_v6_host
 *	\brief			ホスト名からIPv6を取得する
 *	\param [out]	saddr		ソケットアドレス。sockaddr_storage推奨？
 *	\param [in]	host		ホスト名
 *	\return		成功時0。失敗時-1
 *	\remarks		成功したらfamilyは書き換わります
 */
int			sockaddr_set_v6_host(struct sockaddr *saddr, char *host);

/**
 *	\fn				sockaddr_set_any_host
 *	\brief			ホスト名からIPv4/IPv6を取得する
 *	\param [out]	saddr		ソケットアドレス。sockaddr_storage推奨？
 *	\param [in]	host		ホスト名
 *	\return		成功時0。失敗時-1
 *	\remarks		成功したらfamilyは書き換わります
 */
int			sockaddr_set_any_host(struct sockaddr *saddr, char *host);

/**
 *	\fn				sockaddr_set_v4_host
 *	\brief			宛先の文字列表現を取得します
 *	\param [in]	saddr		ソケットアドレス。sockaddr_storage推奨？
 *	\param [out]	dst_host	書き込み先バッファ
 *	\param [in]	dst_len		dst_hostの長さ
 *	\return		成功時は書き込んだ長さ。失敗時マイナス。あとerrno。
 */
ssize_t		sockaddr_get_dst_str(struct sockaddr *saddr, char *dst_host, size_t dst_len);


//ポート周り
/**
 *	\fn				sockaddr_set_port
 *	\brief			ソケットアドレスのポート番号を変更します
 *	\param [in]	saddr		ソケットアドレス。sockaddr_storage推奨？
 *	\param [in]	port		ポート番号。ホストエンディアンでOK
 *	\return		成功時0。失敗時-1。あとerrno
 *	\remarks		AF_UNIXでは無効だお
 */
int			sockaddr_set_port(struct sockaddr *saddr, uint16_t port);

/**
 *	\fn				sockaddr_get_port
 *	\brief			ソケットアドレスのポート番号を取得します
 *	\param [in]	saddr		ソケットアドレス。sockaddr_storage推奨？
 *	\return		ポート番号。ただしAF_INET系列じゃない場合は0のうえ、errno。
 *	\remarks		AF_UNIXでは無効だお
 */
uint16_t	sockaddr_get_port(struct sockaddr *saddr);

//比較
/**
 *	\fn				sockaddr_compare_total
 *	\brief			ソケットアドレスを一挙に全部比較する。
 *	\param [in]	saddr_a	比較A
 *	\param [in]	saddr_b	比較B
 *	\return		saddr_a - saddr_bを基とした比較結果
 *	\remarks		family->addr->portの順番。
 */
int			sockaddr_compare_total(struct sockaddr *saddr_a, struct sockaddr *saddr_b);

//アドレス比較。ファミリ→
/**
 *	\fn				sockaddr_compare_addr
 *	\brief			ソケットアドレスの、アドレス部分の比較
 *	\param [in]	saddr_a	比較A
 *	\param [in]	saddr_b	比較B
 *	\return		saddr_a - saddr_bを基とした比較結果
 *	\remarks		family->addrの順番。射影アドレスはまだ検討していない。
 */
int			sockaddr_compare_addr(struct sockaddr *saddr_a, struct sockaddr *saddr_b);

/**
 *	\fn				sockaddr_compare_port
 *	\brief			ソケットアドレスの、ポート番号の比較
 *	\param [in]	saddr_a	比較A
 *	\param [in]	saddr_b	比較B
 *	\return		saddr_a - saddr_bを基とした比較結果
 *	\remarks		INET系列ならv4 vs v6の比較も一応可能
 */
int			sockaddr_compare_port(struct sockaddr *saddr_a, struct sockaddr *saddr_b);

//射影関連

/**
 *	\fn				sockaddr_storage_4to6
 *	\brief			IPv4ソケットアドレスをIPv6(IPv4射影)に変換する
 *	\param			saddr	変換したい奴
 *	\return		成功時0。失敗時-1のうえerrno
 *	\remarks		絶対に、sockaddr_storageでやること！
 */
int			sockaddr_storage_4to6(struct sockaddr_storage *saddr);

/**
 *	\fn				sockaddr_storage_6to4
 *	\brief			IPv6(IPv4射影)ソケットアドレスをIPv4に変換する
 *	\param			saddr	変換したい奴
 *	\return		成功時0。失敗時-1のうえerrno
 */
int			sockaddr_storage_6to4(struct sockaddr_storage *saddr);

/**
 *	\fn				sockaddr_is_ipv4
 *	\brief			IPv4ソケットアドレスかどうかを調べる
 *	\param			saddr			検査対象
 *	\param			include_mapped	0で、射影アドレスは除外。1で射影アドレスの検査をする
 *	\return		IPv4ならtrue。
 */
int			sockaddr_is_ipv4(struct sockaddr *saddr, int include_mapped);
#endif	/* !SOCKADDR_UTILS_H */
