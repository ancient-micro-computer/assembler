#include <iostream>
#include "assembler.h"
#include "lib.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

using namespace std;

extern char aryVars[512][1024];

//レジスタ
ST_REGISTER register_list[] = {
								{"PC",	REG_TYPE_SPECIAL},
								{"SP",	REG_TYPE_SPECIAL},
								{"CCR",	REG_TYPE_SPECIAL},
								{"INT",	REG_TYPE_SPECIAL},
								{"_PC",	REG_TYPE_SPECIAL},
								{"R0",	REG_TYPE_GENERAL},
								{"R1",	REG_TYPE_GENERAL},
								{"R2",	REG_TYPE_GENERAL},
								{"R3",	REG_TYPE_GENERAL},
								{"R4",	REG_TYPE_GENERAL},
								{"R5",	REG_TYPE_GENERAL},
								{"R6",	REG_TYPE_GENERAL},
								{"R7",	REG_TYPE_GENERAL},
								{"R8",	REG_TYPE_GENERAL},
								{"R9",	REG_TYPE_GENERAL},
								{"R10",	REG_TYPE_GENERAL},
								{"R11",	REG_TYPE_GENERAL},
								{"R12",	REG_TYPE_GENERAL},
								{"R13",	REG_TYPE_GENERAL},
								{"R14",	REG_TYPE_GENERAL},
								{"R15",	REG_TYPE_GENERAL},
								{""},
							};
//ニーモニックリスト
ST_MNEMONIC mnemonic_list[] = {
							//   オペコード名,	語長,	オペランド
								{"NOP",         1,         ""},
								{"HALT",        1,         ""},
								{"ADD",         3,         "rv"},
								{"ADD",         3,         "rr"},
								{"ADC",         3,         "rv"},
								{"ADC",         3,         "rr"},
								{"SUB",         3,         "rv"},
								{"SUB",         3,         "rr"},
								{"SUBC",        3,         "rv"},
								{"SUBC",        3,         "rr"},
								{"MUL",         3,         "rv"},
								{"MUL",         3,         "rr"},
								{"DIV",         3,         "rv"},
								{"DIV",         3,         "rr"},
								{"INC",         2,         "r"},
								{"DEC",         2,         "r"},
								{"CMP",         3,         "rv"},
								{"CMP",         3,         "rr"},
								{"CMP",         2,         "r"},
								{"SLA",         3,         "rv"},
								{"SRA",         3,         "rv"},
								{"SL",          3,         "rv"},
								{"SR",          3,         "rv"},
								{"AND",         3,         "rv"},
								{"AND",         3,         "rr"},
								{"OR",          3,         "rv"},
								{"OR",          3,         "rr"},
								{"XOR",         3,         "rv"},
								{"XOR",         3,         "rr"},
								{"TST",         3,         "rv"},
								{"SET",         3,         "rv"},
								{"RST",         3,         "rv"},
								{"MOV",         4,         "rmvv"},
								{"MOV",         4,         "rmrv"},
								{"MOV",         3,         "mrr"},
								{"MOV",         3,         "rv"},
								{"MOV",         3,         "rr"},
								{"MOV",         3,         "rs"},
								{"MOV",         3,         "rc"},
								{"PUSH",        2,         "r"},
								{"POP",         2,         "r"},
								{"BR",          2,         "v"},
								{"BR",          3,         "rv"},
								{"BRC",         2,         "v"},
								{"BRNC",        2,         "v"},
								{"BRZ",         2,         "v"},
								{"BRNZ",        2,         "v"},
								{"BRP",         2,         "v"},
								{"BRM",         2,         "v"},
								{"BRPO",        2,         "v"},
								{"BRPE",        2,         "v"},
								{"BRGT",        2,         "v"},
								{"BRLT",        2,         "v"},
								{"BREQ",        2,         "v"},
								{"BRNEQ",       2,         "v"},
								{"CALL",        2,         "v"},
								{"CALL",        2,         "r"},
								{"RET",         1,         ""},
								{"EI",          1,         ""},
								{"DI",          1,         ""},
								{"SWI",         1,         ""},
								{"RETI",        1,         ""},
								{"", 0, ""},
							};
//擬似命令
ST_MNEMONIC ass_op_list [] = {
								{".ORG",        1,         "v"},
								{".DS",         1,         "v"},
								{".DEF",        1,         "vv"},
								{"",			0,         ""},
							};

/*============================================================================*
 *  関数名
 *      main関数
 *  概要
 *      アセンブルを自動で行うプログラム
 *  パラメタ説明
 *      
 *  戻り値
 *      0：正常　負数：異常
 *============================================================================*/
int main(int argc, char **argv){
	//変数宣言
	FILE	*fp;
	char	filename[1024];

	//初期化
	memset(m_infilename, 0x0, sizeof(char) * 1024);
	memset(m_outfilename, 0x0, sizeof(char) * 1024);

	memset(mc_buff, 0, sizeof(unsigned short)*MACHINE_CODE_LIST_MAX);
	g_basis_addr = 0;
	pc = 0;
	start_mc = 0;

	memset(label_def_list, 0x0, sizeof(ST_LABEL_ELEMENT) * LABEL_LIST_MAX);

	log_init();

	strcpy_s(filename, 1024, FILE_NAME);

	printf("Virtual CPU Assembler for CPU Board Simulator Version 1.00 %d %s\n", argc, argv[0]);
	if(argc < 2) {
		printf("アセンブルするファイル名を入力してください。 \n");
		scanf("%s", filename);
		printf("ファイル名: %s\n", filename);
	} else {
		printf("ファイル名: %s\n", argv[1]);
		strcpy_s(filename, 1024, argv[1]);
		strcpy_s(m_infilename, 1024, filename);
	}

	//ファイルオープン
	if((fp = fopen(filename,"r")) != 0 ){
		printf("ファイルオープンに失敗しました。\n");
		return -1;
	}
	log(LOG_INFO,"file open OK\n");
	
	//アセンブル処理
	assemble(fp);
	fclose(fp);
	//ファイル出力(～.asm → ～.dat)
	Split(filename, ".");
	strcpy_s(filename, 1024, aryVars[0]);
	strcat_s(filename, 1024, ".dat");

	strcpy_s(m_outfilename, 1024, filename);

	//ファイルオープン
	if((fp = fopen(filename,"w")) != 0 ){
		printf("ファイルオープンに失敗しました。\n");
		return -2;
	}

	output_file(fp, start_mc, pc, mc_buff);
	fclose(fp);

	log_close();
	
	return 0;
}

/*============================================================================*
 *  関数名
 *      アセンブル処理
 *  概要
 *      アセンブラコードをアセンブルする
 *  パラメタ説明
 *      FILE		*fp			ソースコード
 *  戻り値
 *      なし
 *============================================================================*/
int assemble(FILE *fp){
	char	op_pattern[STRING_LEN];
	char	buff[STRING_LEN];			//ファイル読み込み用バッファ
	char	work_str[STRING_LEN];		//
	int		split_cnt;					//分割数
	int		mnemonic_ind;				//分割要素アクセス用
	int		mc_op_ind;					//マシンコードバッファアクセス用
	int		output_mc_flg = 0;			//出力スタート値設定判別用
	int		fake_op_flg;				//擬似命令判別用
	int		lbl_def_flg;
	int		i,s;
	int		reg_flg = 0;
	int		lcnt;
	int		opcode_idx;
	int		last_pc, tmppc, code_chk;
	char*	cur_mnemonic;
	int		macro_recursive_block;		// マクロ循環参照防止チェックカウンタ
	char*	nexttok = NULL;

	log(LOG_INFO,"Assemble First analysis start\n");
	//
	// １次解析関数
	//
	while(!feof(fp)){
		//初期化
		fake_op_flg = 0;
		lbl_def_flg = 0;
		//配列
		memset(buff, 0, sizeof(char)*STRING_LEN);
		memset(work_str, 0, sizeof(char)*STRING_LEN);

		//1行の文字列を取得・大文字に変換
		fgets(buff, STRING_LEN-1, fp);
		log(LOG_INFO,"GET line: %s\n",buff);

		for(i = 0; i < STRING_LEN && buff[i] != '\0'; i++){
			buff[i] = toupper(buff[i]);
		}
		//コメント・tab文字を変換
		StrScrape(buff, work_str);

		//文字列分割
		split_cnt = Split(work_str, " ");
		//分割が0個の場合
		//if(split_cnt == 0){ split_cnt++; }

		//ラベル定義解析
		for(mnemonic_ind = 0; mnemonic_ind < split_cnt; mnemonic_ind++){
			cur_mnemonic = aryVars[mnemonic_ind];
			switch(analysis(cur_mnemonic)){
				case TYPE_LABEL_DEF:		//ラベルの定義
					for(i = 0; i < LABEL_LIST_MAX; i++){
						//中間データへ出力（定義）
						if(strcmp(label_def_list[i].label_name, "") == 0){
							strcpy_s(label_def_list[i].label_name, 128, strtok_s(aryVars[0], ":", &nexttok));
							label_def_list[i].addr = g_basis_addr;
							log(LOG_OBJ,"$%08x\t%s\n",label_def_list[i].addr,label_def_list[i].label_name);
							lbl_def_flg = 1;
							break;
						}
					}
					break;

				case TYPE_FAKE_OP:				//擬似命令
					//擬似命令解釈
					if(strcmp(aryVars[0], ".ORG") == 0){
					//.ORG [アドレス（即値）]
						if((split_cnt == 2) && (isNumeric(aryVars[1]) == 1)){
							g_basis_addr = Hex(aryVars[1]);
							pc = g_basis_addr;
							fake_op_flg = 1;
						} else {
							log(LOG_ERROR,".ORG命令　オペランドエラー %s\n", work_str);
						}
					}
					if(strcmp(aryVars[0], ".DS") == 0){
					//.DS [サイズ] [初期値]		// [サイズ]で示された領域を[初期値]で埋める
						//オペランド数と整数チェック
						if((split_cnt == 3) && (isNumeric(aryVars[1]) == 1) && (isNumeric(aryVars[2]) == 1)) {
							s = g_basis_addr + Hex(aryVars[1]);
							//初期化領域閾値チェック
							if(s >= MACHINE_CODE_LIST_MAX){
								log(LOG_ERROR,".DS命令　メモリオーバー メモリサイズ'%d' ", MACHINE_CODE_LIST_MAX);
							}else if(Hex(aryVars[1]) <= 0){
								log(LOG_ERROR,".DS命令　サイズ定義エラー", MACHINE_CODE_LIST_MAX);
							}

							//領域初期化
							lcnt = Hex(aryVars[1]);
							for(i = 0; i < lcnt; i++) {
								mc_buff[g_basis_addr + i] = (unsigned short)Hex(aryVars[2]);
							}
							g_basis_addr += lcnt;
							pc = g_basis_addr;
							fake_op_flg = 1;
						}
					}
					if(strcmp(aryVars[0], ".DEF") == 0) {
						if((split_cnt == 3) && (isNumeric(aryVars[1]) == 0)) {
							for(i = 0; i < LABEL_LIST_MAX; i++){
								//中間データへ出力（定義）
								if(strcmp(label_def_list[i].label_name, "") == 0){
									strcpy_s(label_def_list[i].label_name, 128, aryVars[1]);
									strcpy_s(label_def_list[i].macro, 128, aryVars[2]);
									break;
								}
							}
						}
					}
					break;

				case TYPE_OPERATION:		//命令
				case TYPE_LABEL_REF:		//ラベル参照
				case TYPE_REGISTER:			//レジスタ
				case TYPE_MEM_R:			//アドレス(レジスタ)
				case TYPE_MEM_V:			//アドレス(即値)
				case TYPE_VAL:				//即値
					g_basis_addr++;
					break;

				//アセンブルエラー
				default:
					log(LOG_ERROR,"Assemble error %s\n",buff);
					return ASSEMBLE_ERROR;
			}
			// 擬似命令である場合は、既にswitch文内で解析済みの為
			// ループを抜ける
			if(fake_op_flg == 1){
				break;
			}
		}
	}
	log(LOG_INFO,"Assemble Second analysis start\n");
	log(LOG_OBJ,"Second analysis start----------\n");
	//
	// ２次解析関数
	//
	rewind(fp);
	while(!feof(fp)){
		//初期化
		mc_op_ind = 0;
		fake_op_flg = 0;
		lbl_def_flg = 0;
		//配列
		memset(buff, 0, sizeof(char)*STRING_LEN);
		memset(work_str, 0, sizeof(char)*STRING_LEN);
		memset(op_pattern, 0, sizeof(char)*STRING_LEN);

		//1行の文字列を取得・大文字に変換
		fgets(buff, STRING_LEN-1, fp);
		for(i = 0; i < STRING_LEN && buff[i] != (int)NULL; i++){
			buff[i] = toupper(buff[i]);
		}

/*		//ラベル定義行の読み飛ばし
		if(strstr(buff, ":") != NULL){
			continue;
		}
*/
		//コメント・tab文字を変換
		StrScrape(buff, work_str);

		//文字列分割
		split_cnt = Split(work_str, " ");
		
		//ラベル定義以外の解析
		last_pc = pc;
		macro_recursive_block = 0;
		for(mnemonic_ind = 0, opcode_idx = 0; mnemonic_ind < split_cnt; mnemonic_ind++){
			cur_mnemonic = aryVars[mnemonic_ind];

RE_ANALYSIS:
			switch(analysis(cur_mnemonic)){
				case TYPE_LABEL_DEF:		//ラベルの定義
					lbl_def_flg = 1;
					break;

				case TYPE_OPERATION:		//命令
					opcode_idx = mnemonic_ind;
					mc_op_ind = pc;
					pc++;
					break;

				case TYPE_LABEL_REF:		//ラベル参照
					for(i = 0; i < LABEL_LIST_MAX; i++){
						if(strcmp(label_def_list[i].label_name, cur_mnemonic) == 0) {
							if(strcmp(label_def_list[i].macro, "") == 0) {
								// ラベル値 置換
								mc_buff[pc] = label_def_list[i].addr;
								pc++;
								break;
							} else {
								// マクロ値 置換＆再解析
								cur_mnemonic = label_def_list[i].macro;
								if(macro_recursive_block < MACRO_MAX_RECURSIVE_REFER) {
									macro_recursive_block++;
									goto RE_ANALYSIS;
								} else {
									log(LOG_ERROR,"マクロ 再帰参照オーバーエラー%s\n",buff);
									return RECURSIVEOVER_ERROR;
								}
							}
						}
						if(i >= LABEL_LIST_MAX){
							//ラベル定義エラー
							log(LOG_ERROR,"ラベル　未定義エラー %s\n",buff);
							return ASSEMBLE_ERROR;
						}
					}
					strcat_s(op_pattern, 2048, "v");
					break;

				case TYPE_REGISTER:			//レジスタ
					//レジスタ"R?"
					for(i = 0; strcmp(register_list[i].reg_name, "") != 0; i++){
						if(strcmp(register_list[i].reg_name, cur_mnemonic) == 0){
							//ユーザーレジスタであった場合は0～15のレジスタ番号に変換する
							if(register_list[i].reg_type == REG_TYPE_GENERAL){
								i -= 5;
							} else if(register_list[i].reg_type == REG_TYPE_SPECIAL) {
								i += 0x80;
							}
							mc_buff[pc] = i;
							pc++;
							reg_flg = 1;
							break;
						}
					}
					if(reg_flg == 0){
						log(LOG_ERROR,"存在しないレジスタを使用しています %s\n",buff);
						return ASSEMBLE_ERROR;
					}
					strcat_s(op_pattern, 2048, "r");
					break;

				case TYPE_MEM_R:			//アドレス(レジスタ)
					//レジスタ"R?"
					for(i = 0; strcmp(register_list[i].reg_name, "") != 0; i++){
						if(strstr(cur_mnemonic, register_list[i].reg_name) != NULL){
							//ユーザーレジスタであった場合は0～15のレジスタ番号に変換する
							if(register_list[i].reg_type == REG_TYPE_GENERAL){
								i -= 5;
							}
							mc_buff[pc] = i;
							pc++;						
							break;
						}
					}
					strcat_s(op_pattern, 2048, "mr");
					break;

				case TYPE_MEM_V:			//アドレス(即値)
					//即値
					if(isNumeric(cur_mnemonic) == 0){
						log(LOG_ERROR,"アドレス値が数値ではありません %s\n",buff);
					}
					//10進　と　16進により処理を分ける。
					mc_buff[pc] = (unsigned short)Hex(cur_mnemonic);
					
					pc++;
					strcat_s(op_pattern, 2048, "mv");
					break;

				case TYPE_VAL:				//即値
					//即値
					if(isNumeric(cur_mnemonic) == 0){
						log(LOG_ERROR,"アドレス値が数値ではありません %s\n",buff);
					}
					mc_buff[pc] = (unsigned short)Hex(cur_mnemonic);
					pc++;
					strcat_s(op_pattern, 2048, "v");
					break;
				case TYPE_FAKE_OP:
					if(strcmp(aryVars[0], ".ORG") == 0){
					//.ORG [アドレス（即値）]
						if((split_cnt == 2) && (isNumeric(aryVars[1]) == 1)){
							g_basis_addr = Hex(aryVars[1]);
							pc = g_basis_addr;
							fake_op_flg = 1;
						} else {
							log(LOG_ERROR,".ORG命令　オペランドエラー %s\n", work_str);
						}
					}
					if(strcmp(aryVars[0], ".DS") == 0){
						unsigned long inittmp = 0x0;
						//.DS [サイズ] [初期値]
						//オペランド数と整数チェック
						if(split_cnt == 3 && isNumeric(aryVars[1]) == 1) {
							if(analysis(aryVars[2]) == TYPE_LABEL_REF) {
								for(i = 0; i < LABEL_LIST_MAX; i++){
									if(strcmp(label_def_list[i].label_name, aryVars[2]) == 0){
										s = pc + Hex(aryVars[1]);
										inittmp = label_def_list[i].addr;
										break;
									}
									if(i >= LABEL_LIST_MAX) {
										//ラベル定義エラー
										log(LOG_ERROR,"ラベル　未定義エラー %s\n",buff);
										return ASSEMBLE_ERROR;
									}
								}
							} else if (isNumeric(aryVars[2]) == 1) {
								s = pc + Hex(aryVars[1]);
								inittmp = Hex(aryVars[2]);
							} else {
								log(LOG_ERROR,".DS 文法エラー %s\n",buff);
								return ASSEMBLE_ERROR;
							}

							//初期化領域閾値チェック
							if(s >= MACHINE_CODE_LIST_MAX){
								log(LOG_ERROR,".DS命令　メモリオーバー メモリサイズ'%d'\n", MACHINE_CODE_LIST_MAX);
							}else if(Hex(aryVars[1]) <= 0){
								log(LOG_ERROR,".DS命令　サイズ定義エラー\n", MACHINE_CODE_LIST_MAX);
							}

							//領域初期化
							lcnt = Hex(aryVars[1]);
							for(i = 0; i < lcnt; i++) {
								mc_buff[pc + i] = (unsigned short)inittmp;
							}

							//プログラムカウンタ更新
							pc = s;
							fake_op_flg = 1;
						}
					}
					if(strcmp(aryVars[0], ".DEF") == 0){
						fake_op_flg = 1;
					}
					break;

				//アセンブルエラー
				default:
					log(LOG_ERROR,"ニーモニックがどの種別にも属しません %s\n",buff);
					return ASSEMBLE_ERROR;
			}
			// 擬似命令である場合は、既にswitch文内で解析済みの為
			// ループを抜ける
			if(fake_op_flg == 1){
				break;
			}
		}

		//命令マシンコード解析
		if(split_cnt && fake_op_flg == 0) {
			code_chk = 0;
			for(i = 0; strcmp(mnemonic_list[i].op_code_name, "") != 0; i++){
				if(strcmp(mnemonic_list[i].op_code_name, aryVars[opcode_idx]) == 0){
					if(strcmp(mnemonic_list[i].param, op_pattern) == 0){
						mc_buff[mc_op_ind] = i;
						//デバッグ用ログ出力
						log(LOG_OBJ,"$%08x %s [",mc_op_ind, work_str);
						for(tmppc = last_pc; tmppc < pc; tmppc++) {
							log(LOG_OBJ, "$%04x ", mc_buff[tmppc]);
						}
						log(LOG_OBJ, "]\n");

						if(output_mc_flg == 0){
							start_mc = mc_op_ind;
							output_mc_flg = 1;
						}

						code_chk = 1;
						break;
					}
				}
			}
			if(fake_op_flg == 0 && lbl_def_flg == 0 && code_chk == 0) {
				log(LOG_ERROR,".ORG命令　ニーモニックエラー %s\n", work_str);
				log(LOG_OBJ,"$%08x %s MNEMONIC ERROR.\n",mc_op_ind, work_str);
				return ASSEMBLE_ERROR;
			}
		}
	}
	return 0;
}
/*============================================================================*
 *  関数名
 *      ソースコード解析
 *  概要
 *      文字列から命令種別結果を返す
 *  パラメタ説明
 *      
 *  戻り値
 *　　　　0: エラー  0以外の整数:　種別
 *============================================================================*/
int analysis(char *mnemonic){
	char *ptr = mnemonic;
	int i;

	//即値
	if(isNumeric(ptr) == 1){
		return TYPE_VAL;
	}

	//命令
	for(i = 0; strcmp(mnemonic_list[i].op_code_name, ""); i++){
		if(strcmp(mnemonic_list[i].op_code_name, ptr) == 0){
			return TYPE_OPERATION;
		}
	}
	//擬似命令
	for(i = 0; strcmp(ass_op_list[i].op_code_name, ""); i++){
		if(strcmp(ass_op_list[i].op_code_name, ptr) == 0){
			return TYPE_FAKE_OP;
		}
	}
	//レジスタ"R?"
	for(i = 0; strcmp(register_list[i].reg_name, ""); i++){
		if(strcmp(register_list[i].reg_name, ptr) == 0){
			return TYPE_REGISTER;
		}
	}
	//メモリ"*"
	if(*ptr == '*'){
		//レジスタ
		if(strstr(ptr, "R") != NULL){
			return TYPE_MEM_R;
		}else{
		//即値
			return TYPE_MEM_V;
		}
	}

	//ラベル定義
	if(strstr(ptr, ":") != NULL){
		//ラベル名中間データへ登録
		return TYPE_LABEL_DEF;
	} else {
		return TYPE_LABEL_REF;
	}

	return 0;
}

/*============================================================================*
 *  関数名
 *      アセンブル結果ファイル出力
 *  概要
 *      指定したファイルにアセンブル結果を出力する
 *  パラメタ説明
 *      
 *  戻り値
 *　　　　成功時 : 1   　失敗時 : 0
 *      
 *============================================================================*/
int output_file(FILE *fp, int start, int end, unsigned short *mem){
	int		ret = 0;
	int		i, j;
	int		width, size;
	int		over;

	width = 16;
	size = end - start;
	//ファイル出力

	for(i = start_mc, j = 0; j < size; i++, j++) {
		if(((j % 8) == 0) && (j > 0)){
			fprintf(fp, "\n");
		}
		// アドレス出力
		if ( (j & 0x7) == 0 ) {
			fprintf(fp, "$%08x ", i);
		}
		// データ出力
		fprintf(fp, "$%04x ", *(mem+i));
	}
	over = (8 - size % 8);
	if(over % 8 > 0) {
		for( ;over > 0; over--){
				fprintf(fp, "$%04x ", 0x0000);
		}
	}
	return ret;
}
