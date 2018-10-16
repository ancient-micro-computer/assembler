#include "assembler.h"
#include "lib.h"

using namespace Simulator;

extern char aryVars[512][1024];

//���W�X�^
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
//�j�[���j�b�N���X�g
ST_MNEMONIC mnemonic_list[] = {
							//   �I�y�R�[�h��,	�꒷,	�I�y�����h
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
//�[������
ST_MNEMONIC ass_op_list [] = {
								{".ORG",        1,         "v"},
								{".DS",         1,         "v"},
								{".DEF",        1,         "vv"},
								{"",			0,         ""},
							};

Assembler::Assembler()
{
	memset(m_infilename, 0x0, sizeof(char) * 1024);
	memset(m_outfilename, 0x0, sizeof(char) * 1024);

	memset(mc_buff, 0, sizeof(unsigned short)*MACHINE_CODE_LIST_MAX);
	g_basis_addr = 0;
	pc = 0;
	start_mc = 0;

	memset(label_def_list, 0x0, sizeof(ST_LABEL_ELEMENT) * LABEL_LIST_MAX);

	log_init();
}

Assembler::~Assembler()
{
	log_close();
}

/*============================================================================*
 *  �֐���
 *      main�֐�
 *  �T�v
 *      �A�Z���u���������ōs���v���O����
 *  �p�����^����
 *      
 *  �߂�l
 *      0�F����@�����F�ُ�
 *============================================================================*/
int Assembler::asmmain(int argc, char *fn){
	//�ϐ��錾
	FILE	*fp;
	char	filename[1024];

	//������
	strcpy_s(filename, 1024, FILE_NAME);

	printf("Virtual CPU Assembler for CPU Board Simulator [TOH] Version 1.00\n");
	if(argc < 2) {
		printf("Toh�@�A�Z���u��\n�A�Z���u������t�@�C��������͂��Ă��������B");
		scanf_s("%s",filename);
	} else {
		strcpy_s(filename, 1024, fn);
		strcpy_s(m_infilename, 1024, filename);
	}

	//�t�@�C���I�[�v��
	if((fopen_s(&fp, filename,"r")) != 0 ){
		printf("�t�@�C���I�[�v���Ɏ��s���܂����B");
		return -1;
	}
	log(LOG_INFO,"file open OK\n");
	
	//�A�Z���u������
	toh_assemble(fp);
	fclose(fp);
	//�t�@�C���o��(�`.asm �� �`.dat)
	Split(filename, ".");
	strcpy_s(filename, 1024, aryVars[0]);
	strcat_s(filename, 1024, ".dat");

	strcpy_s(m_outfilename, 1024, filename);

	//�t�@�C���I�[�v��
	if((fopen_s(&fp, filename,"w")) != 0 ){
		printf("�t�@�C���I�[�v���Ɏ��s���܂����B");
		exit(1);
	}

	toh_output_file(fp, start_mc, pc, mc_buff);
	fclose(fp);

	return 0;
}

/*============================================================================*
 *  �֐���
 *      �A�Z���u������
 *  �T�v
 *      �A�Z���u���R�[�h���A�Z���u������
 *  �p�����^����
 *      FILE		*fp			�\�[�X�R�[�h
 *  �߂�l
 *      �Ȃ�
 *============================================================================*/
int Assembler::toh_assemble(FILE *fp){
	char	op_pattern[STRING_LEN];
	char	buff[STRING_LEN];			//�t�@�C���ǂݍ��ݗp�o�b�t�@
	char	work_str[STRING_LEN];		//
	int		split_cnt;					//������
	int		mnemonic_ind;				//�����v�f�A�N�Z�X�p
	int		mc_op_ind;					//�}�V���R�[�h�o�b�t�@�A�N�Z�X�p
	int		output_mc_flg = 0;			//�o�̓X�^�[�g�l�ݒ蔻�ʗp
	int		fake_op_flg;				//�[�����ߔ��ʗp
	int		lbl_def_flg;
	int		i,s;
	int		reg_flg = 0;
	int		lcnt;
	int		opcode_idx;
	int		last_pc, tmppc, code_chk;
	char*	cur_mnemonic;
	int		macro_recursive_block;		// �}�N���z�Q�Ɩh�~�`�F�b�N�J�E���^
	char*	nexttok = NULL;

	log(LOG_INFO,"Assemble First analysis start\n");
	//
	// �P����͊֐�
	//
	while(!feof(fp)){
		//������
		fake_op_flg = 0;
		lbl_def_flg = 0;
		//�z��
		memset(buff, 0, sizeof(char)*STRING_LEN);
		memset(work_str, 0, sizeof(char)*STRING_LEN);

		//1�s�̕�������擾�E�啶���ɕϊ�
		fgets(buff, STRING_LEN-1, fp);
		log(LOG_INFO,"GET line: %s\n",buff);

		for(i = 0; i < STRING_LEN && buff[i] != '\0'; i++){
			buff[i] = toupper(buff[i]);
		}
		//�R�����g�Etab������ϊ�
		StrScrape(buff, work_str);

		//�����񕪊�
		split_cnt = Split(work_str, " ");
		//������0�̏ꍇ
		//if(split_cnt == 0){ split_cnt++; }

		//���x����`���
		for(mnemonic_ind = 0; mnemonic_ind < split_cnt; mnemonic_ind++){
			cur_mnemonic = aryVars[mnemonic_ind];
			switch(toh_analysis(cur_mnemonic)){
				case TYPE_LABEL_DEF:		//���x���̒�`
					for(i = 0; i < LABEL_LIST_MAX; i++){
						//���ԃf�[�^�֏o�́i��`�j
						if(strcmp(label_def_list[i].label_name, "") == 0){
							strcpy_s(label_def_list[i].label_name, 128, strtok_s(aryVars[0], ":", &nexttok));
							label_def_list[i].addr = g_basis_addr;
							log(LOG_OBJ,"$%08x\t%s\n",label_def_list[i].addr,label_def_list[i].label_name);
							lbl_def_flg = 1;
							break;
						}
					}
					break;

				case TYPE_FAKE_OP:				//�[������
					//�[�����߉���
					if(strcmp(aryVars[0], ".ORG") == 0){
					//.ORG [�A�h���X�i���l�j]
						if((split_cnt == 2) && (isNumeric(aryVars[1]) == 1)){
							g_basis_addr = Hex(aryVars[1]);
							pc = g_basis_addr;
							fake_op_flg = 1;
						} else {
							log(LOG_ERROR,".ORG���߁@�I�y�����h�G���[ %s\n", work_str);
						}
					}
					if(strcmp(aryVars[0], ".DS") == 0){
					//.DS [�T�C�Y] [�����l]		// [�T�C�Y]�Ŏ����ꂽ�̈��[�����l]�Ŗ��߂�
						//�I�y�����h���Ɛ����`�F�b�N
						if((split_cnt == 3) && (isNumeric(aryVars[1]) == 1) && (isNumeric(aryVars[2]) == 1)) {
							s = g_basis_addr + Hex(aryVars[1]);
							//�������̈�臒l�`�F�b�N
							if(s >= MACHINE_CODE_LIST_MAX){
								log(LOG_ERROR,".DS���߁@�������I�[�o�[ �������T�C�Y'%d' ", MACHINE_CODE_LIST_MAX);
							}else if(Hex(aryVars[1]) <= 0){
								log(LOG_ERROR,".DS���߁@�T�C�Y��`�G���[", MACHINE_CODE_LIST_MAX);
							}

							//�̈揉����
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
								//���ԃf�[�^�֏o�́i��`�j
								if(strcmp(label_def_list[i].label_name, "") == 0){
									strcpy_s(label_def_list[i].label_name, 128, aryVars[1]);
									strcpy_s(label_def_list[i].macro, 128, aryVars[2]);
									break;
								}
							}
						}
					}
					break;

				case TYPE_OPERATION:		//����
				case TYPE_LABEL_REF:		//���x���Q��
				case TYPE_REGISTER:			//���W�X�^
				case TYPE_MEM_R:			//�A�h���X(���W�X�^)
				case TYPE_MEM_V:			//�A�h���X(���l)
				case TYPE_VAL:				//���l
					g_basis_addr++;
					break;

				//�A�Z���u���G���[
				default:
					log(LOG_ERROR,"Assemble error %s\n",buff);
					return ASSEMBLE_ERROR;
			}
			// �[�����߂ł���ꍇ�́A����switch�����ŉ�͍ς݂̈�
			// ���[�v�𔲂���
			if(fake_op_flg == 1){
				break;
			}
		}
	}
	log(LOG_INFO,"Assemble Second analysis start\n");
	log(LOG_OBJ,"Second analysis start----------\n");
	//
	// �Q����͊֐�
	//
	rewind(fp);
	while(!feof(fp)){
		//������
		mc_op_ind = 0;
		fake_op_flg = 0;
		lbl_def_flg = 0;
		//�z��
		memset(buff, 0, sizeof(char)*STRING_LEN);
		memset(work_str, 0, sizeof(char)*STRING_LEN);
		memset(op_pattern, 0, sizeof(char)*STRING_LEN);

		//1�s�̕�������擾�E�啶���ɕϊ�
		fgets(buff, STRING_LEN-1, fp);
		for(i = 0; i < STRING_LEN && buff[i] != NULL; i++){
			buff[i] = toupper(buff[i]);
		}

/*		//���x����`�s�̓ǂݔ�΂�
		if(strstr(buff, ":") != NULL){
			continue;
		}
*/
		//�R�����g�Etab������ϊ�
		StrScrape(buff, work_str);

		//�����񕪊�
		split_cnt = Split(work_str, " ");
		
		//���x����`�ȊO�̉��
		last_pc = pc;
		macro_recursive_block = 0;
		for(mnemonic_ind = 0, opcode_idx = 0; mnemonic_ind < split_cnt; mnemonic_ind++){
			cur_mnemonic = aryVars[mnemonic_ind];

RE_ANALYSIS:
			switch(toh_analysis(cur_mnemonic)){
				case TYPE_LABEL_DEF:		//���x���̒�`
					lbl_def_flg = 1;
					break;

				case TYPE_OPERATION:		//����
					opcode_idx = mnemonic_ind;
					mc_op_ind = pc;
					pc++;
					break;

				case TYPE_LABEL_REF:		//���x���Q��
					for(i = 0; i < LABEL_LIST_MAX; i++){
						if(strcmp(label_def_list[i].label_name, cur_mnemonic) == 0) {
							if(strcmp(label_def_list[i].macro, "") == 0) {
								// ���x���l �u��
								mc_buff[pc] = label_def_list[i].addr;
								pc++;
								break;
							} else {
								// �}�N���l �u�����ĉ��
								cur_mnemonic = label_def_list[i].macro;
								if(macro_recursive_block < MACRO_MAX_RECURSIVE_REFER) {
									macro_recursive_block++;
									goto RE_ANALYSIS;
								} else {
									log(LOG_ERROR,"�}�N�� �ċA�Q�ƃI�[�o�[�G���[%s\n",buff);
									return RECURSIVEOVER_ERROR;
								}
							}
						}
						if(i >= LABEL_LIST_MAX){
							//���x����`�G���[
							log(LOG_ERROR,"���x���@����`�G���[ %s\n",buff);
							return ASSEMBLE_ERROR;
						}
					}
					strcat_s(op_pattern, 2048, "v");
					break;

				case TYPE_REGISTER:			//���W�X�^
					//���W�X�^"R?"
					for(i = 0; strcmp(register_list[i].reg_name, "") != 0; i++){
						if(strcmp(register_list[i].reg_name, cur_mnemonic) == 0){
							//���[�U�[���W�X�^�ł������ꍇ��0�`15�̃��W�X�^�ԍ��ɕϊ�����
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
						log(LOG_ERROR,"���݂��Ȃ����W�X�^���g�p���Ă��܂� %s\n",buff);
						return ASSEMBLE_ERROR;
					}
					strcat_s(op_pattern, 2048, "r");
					break;

				case TYPE_MEM_R:			//�A�h���X(���W�X�^)
					//���W�X�^"R?"
					for(i = 0; strcmp(register_list[i].reg_name, "") != 0; i++){
						if(strstr(cur_mnemonic, register_list[i].reg_name) != NULL){
							//���[�U�[���W�X�^�ł������ꍇ��0�`15�̃��W�X�^�ԍ��ɕϊ�����
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

				case TYPE_MEM_V:			//�A�h���X(���l)
					//���l
					if(isNumeric(cur_mnemonic) == 0){
						log(LOG_ERROR,"�A�h���X�l�����l�ł͂���܂��� %s\n",buff);
					}
					//10�i�@�Ɓ@16�i�ɂ�菈���𕪂���B
					mc_buff[pc] = (unsigned short)Hex(cur_mnemonic);
					
					pc++;
					strcat_s(op_pattern, 2048, "mv");
					break;

				case TYPE_VAL:				//���l
					//���l
					if(isNumeric(cur_mnemonic) == 0){
						log(LOG_ERROR,"�A�h���X�l�����l�ł͂���܂��� %s\n",buff);
					}
					mc_buff[pc] = (unsigned short)Hex(cur_mnemonic);
					pc++;
					strcat_s(op_pattern, 2048, "v");
					break;
				case TYPE_FAKE_OP:
					if(strcmp(aryVars[0], ".ORG") == 0){
					//.ORG [�A�h���X�i���l�j]
						if((split_cnt == 2) && (isNumeric(aryVars[1]) == 1)){
							g_basis_addr = Hex(aryVars[1]);
							pc = g_basis_addr;
							fake_op_flg = 1;
						} else {
							log(LOG_ERROR,".ORG���߁@�I�y�����h�G���[ %s\n", work_str);
						}
					}
					if(strcmp(aryVars[0], ".DS") == 0){
						unsigned long inittmp = 0x0;
						//.DS [�T�C�Y] [�����l]
						//�I�y�����h���Ɛ����`�F�b�N
						if(split_cnt == 3 && isNumeric(aryVars[1]) == 1) {
							if(toh_analysis(aryVars[2]) == TYPE_LABEL_REF) {
								for(i = 0; i < LABEL_LIST_MAX; i++){
									if(strcmp(label_def_list[i].label_name, aryVars[2]) == 0){
										s = pc + Hex(aryVars[1]);
										inittmp = label_def_list[i].addr;
										break;
									}
									if(i >= LABEL_LIST_MAX) {
										//���x����`�G���[
										log(LOG_ERROR,"���x���@����`�G���[ %s\n",buff);
										return ASSEMBLE_ERROR;
									}
								}
							} else if (isNumeric(aryVars[2]) == 1) {
								s = pc + Hex(aryVars[1]);
								inittmp = Hex(aryVars[2]);
							} else {
								log(LOG_ERROR,".DS ���@�G���[ %s\n",buff);
								return ASSEMBLE_ERROR;
							}

							//�������̈�臒l�`�F�b�N
							if(s >= MACHINE_CODE_LIST_MAX){
								log(LOG_ERROR,".DS���߁@�������I�[�o�[ �������T�C�Y'%d'\n", MACHINE_CODE_LIST_MAX);
							}else if(Hex(aryVars[1]) <= 0){
								log(LOG_ERROR,".DS���߁@�T�C�Y��`�G���[\n", MACHINE_CODE_LIST_MAX);
							}

							//�̈揉����
							lcnt = Hex(aryVars[1]);
							for(i = 0; i < lcnt; i++) {
								mc_buff[pc + i] = (unsigned short)inittmp;
							}

							//�v���O�����J�E���^�X�V
							pc = s;
							fake_op_flg = 1;
						}
					}
					if(strcmp(aryVars[0], ".DEF") == 0){
						fake_op_flg = 1;
					}
					break;

				//�A�Z���u���G���[
				default:
					log(LOG_ERROR,"�j�[���j�b�N���ǂ̎�ʂɂ������܂��� %s\n",buff);
					return ASSEMBLE_ERROR;
			}
			// �[�����߂ł���ꍇ�́A����switch�����ŉ�͍ς݂̈�
			// ���[�v�𔲂���
			if(fake_op_flg == 1){
				break;
			}
		}

		//���߃}�V���R�[�h���
		if(split_cnt && fake_op_flg == 0) {
			code_chk = 0;
			for(i = 0; strcmp(mnemonic_list[i].op_code_name, "") != 0; i++){
				if(strcmp(mnemonic_list[i].op_code_name, aryVars[opcode_idx]) == 0){
					if(strcmp(mnemonic_list[i].param, op_pattern) == 0){
						mc_buff[mc_op_ind] = i;
						//�f�o�b�O�p���O�o��
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
				log(LOG_ERROR,".ORG���߁@�j�[���j�b�N�G���[ %s\n", work_str);
				log(LOG_OBJ,"$%08x %s MNEMONIC ERROR.\n",mc_op_ind, work_str);
				return ASSEMBLE_ERROR;
			}
		}
	}
	return 0;
}
/*============================================================================*
 *  �֐���
 *      �\�[�X�R�[�h���
 *  �T�v
 *      �����񂩂疽�ߎ�ʌ��ʂ�Ԃ�
 *  �p�����^����
 *      
 *  �߂�l
 *�@�@�@�@0: �G���[  0�ȊO�̐���:�@���
 *============================================================================*/
int Assembler::toh_analysis(char *mnemonic){
	char *ptr = mnemonic;
	int i;

	//���l
	if(isNumeric(ptr) == 1){
		return TYPE_VAL;
	}

	//����
	for(i = 0; strcmp(mnemonic_list[i].op_code_name, ""); i++){
		if(strcmp(mnemonic_list[i].op_code_name, ptr) == 0){
			return TYPE_OPERATION;
		}
	}
	//�[������
	for(i = 0; strcmp(ass_op_list[i].op_code_name, ""); i++){
		if(strcmp(ass_op_list[i].op_code_name, ptr) == 0){
			return TYPE_FAKE_OP;
		}
	}
	//���W�X�^"R?"
	for(i = 0; strcmp(register_list[i].reg_name, ""); i++){
		if(strcmp(register_list[i].reg_name, ptr) == 0){
			return TYPE_REGISTER;
		}
	}
	//������"*"
	if(*ptr == '*'){
		//���W�X�^
		if(strstr(ptr, "R") != NULL){
			return TYPE_MEM_R;
		}else{
		//���l
			return TYPE_MEM_V;
		}
	}

	//���x����`
	if(strstr(ptr, ":") != NULL){
		//���x�������ԃf�[�^�֓o�^
		return TYPE_LABEL_DEF;
	} else {
		return TYPE_LABEL_REF;
	}

	return 0;
}

/*============================================================================*
 *  �֐���
 *      �A�Z���u�����ʃt�@�C���o��
 *  �T�v
 *      �w�肵���t�@�C���ɃA�Z���u�����ʂ��o�͂���
 *  �p�����^����
 *      
 *  �߂�l
 *�@�@�@�@������ : 1   �@���s�� : 0
 *      
 *============================================================================*/
int Assembler::toh_output_file(FILE *fp, int start, int end, unsigned short *mem){
	int		ret = 0;
	int		i, j;
	int		width, size;
	int		over;

	width = 16;
	size = end - start;
	//�t�@�C���o��

	for(i = start_mc, j = 0; j < size; i++, j++) {
		if(((j % 8) == 0) && (j > 0)){
			fprintf(fp, "\n");
		}
		// �A�h���X�o��
		if ( (j & 0x7) == 0 ) {
			fprintf(fp, "$%08x ", i);
		}
		// �f�[�^�o��
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

/*============================================================================*
 *  �֐���
 *      �������擾
 *  �T�v
 *      
 *  �p�����^����
 *      
 *  �߂�l
 *�@�@�@�@
 *============================================================================*/
char*	Assembler::get_infilename()
{
	return m_infilename;
}

char*	Assembler::get_outfilename()
{
	return m_outfilename;
}
