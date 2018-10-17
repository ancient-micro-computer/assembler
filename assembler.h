#include <iostream>
#include "logutil.h"
#include <stdio.h>
#include <string.h>
#include "lib.h"

#define FILE_NAME		"default.asm"
#define BLOC_NAME		"CPU"
#define BLOC_MEM_NAME	"MEM"

#define	REG_TYPE_SPECIAL	0	
#define	REG_TYPE_GENERAL	1

//中間データ　キーワード記録数
#define LABEL_LIST_MAX	2048	// 最大2048ラベル&マクロの記録

//ソース1行の最大文字数
#define STRING_LEN		2048

//マクロの再帰的参照の最大数
#define MACRO_MAX_RECURSIVE_REFER	128

//マシンコード最大格納
#define MACHINE_CODE_LIST_MAX	65536
#define MC_BUFF_MAX				10;

//マシンコード種別
#define TYPE_LABEL_DEF	10
#define TYPE_LABEL_REF	11
#define TYPE_OPERATION	12
#define TYPE_REGISTER	13
#define TYPE_MEM_R		14
#define TYPE_MEM_V		15
#define TYPE_VAL		16
#define TYPE_FAKE_OP	17

//エラーコード
#define FILE_OPEN_ERROR		-1
#define OP_CODE_ERROR		-2
#define OP_PARAM_ERROR		-3
#define MISSING_LABEL		-4
#define MISSING_DEFINE		-5
#define VAGUE_LABEL			-6
#define LABEL_LIST_OVER		-7
#define DEFINE_LIST_OVER	-8
#define ASSEMBLE_ERROR		-9
#define RECURSIVEOVER_ERROR	-10
#define SYSTEM_ERROR		-99

// ------------------------------
// 構造体定義
// ------------------------------
// ------------------------------
// ラベル
// ------------------------------
typedef struct label_element{
		char				label_name[128];		//ラベル名
		int					addr;					//コンパイル結果へのアドレス
		char				macro[512];				//マクロ値
} ST_LABEL_ELEMENT;


// ------------------------------
// ニーモニック
// ------------------------------
typedef struct mnemonic{
	const char *op_code_name;						//オペレーション名
	int len;										//命令語長
	const char *param;								//引数パターンr, v, a, m,（r：レジスタ v：即値 a：アドレス m：メモリ）
} ST_MNEMONIC;

// ------------------------------
// レジスタ
// ------------------------------
typedef struct register_name{
		const char	*reg_name;						//レジスタ名
		int		reg_type;							//レジスタ属性 特殊レジスタ:0　汎用レジスタ:1
} ST_REGISTER;

// ------------------------------
// 関数プロトタイプ
// ------------------------------

#ifndef __ASSEMBLER_H__
#define __ASSEMBLER_H__

#include "typedef.h"
#include "define.h"

using namespace std;

char*	get_infilename();
char*	get_outfilename();

int assemble(FILE *fp);
int analysis(char *mnemonic);
int output_file(FILE *fp, int start, int end, unsigned short *mem);
unsigned short* mc_buff_add(void);

//ベースアドレス
unsigned int g_basis_addr;

//マシンコードバッファ
unsigned short mc_buff[MACHINE_CODE_LIST_MAX];
int	pc;
int start_mc;

//ラベル中間データ
ST_LABEL_ELEMENT label_def_list[LABEL_LIST_MAX];
ST_LABEL_ELEMENT label_ref_list[LABEL_LIST_MAX];

char m_infilename[1024];
char m_outfilename[1024];


#endif  //__ASSEMBLER_H__
