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

//���ԃf�[�^�@�L�[���[�h�L�^��
#define LABEL_LIST_MAX	2048	// �ő�2048���x��&�}�N���̋L�^

//�\�[�X1�s�̍ő啶����
#define STRING_LEN		2048

//�}�N���̍ċA�I�Q�Ƃ̍ő吔
#define MACRO_MAX_RECURSIVE_REFER	128

//�}�V���R�[�h�ő�i�[
#define MACHINE_CODE_LIST_MAX	65536
#define MC_BUFF_MAX				10;

//�}�V���R�[�h���
#define TYPE_LABEL_DEF	10
#define TYPE_LABEL_REF	11
#define TYPE_OPERATION	12
#define TYPE_REGISTER	13
#define TYPE_MEM_R		14
#define TYPE_MEM_V		15
#define TYPE_VAL		16
#define TYPE_FAKE_OP	17

//�G���[�R�[�h
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
// �\���̒�`
// ------------------------------
// ------------------------------
// ���x��
// ------------------------------
typedef struct label_element{
		char				label_name[128];		//���x����
		int					addr;					//�R���p�C�����ʂւ̃A�h���X
		char				macro[512];				//�}�N���l
} ST_LABEL_ELEMENT;


// ------------------------------
// �j�[���j�b�N
// ------------------------------
typedef struct mnemonic{
	char *op_code_name;								//�I�y���[�V������
	int len;										//���ߌ꒷
	char *param;									//�����p�^�[��r, v, a, m,�ir�F���W�X�^ v�F���l a�F�A�h���X m�F�������j
} ST_MNEMONIC;

// ------------------------------
// ���W�X�^
// ------------------------------
typedef struct register_name{
		char	*reg_name;								//���W�X�^��
		int		reg_type;								//���W�X�^���� ���ꃌ�W�X�^:0�@�ėp���W�X�^:1
} ST_REGISTER;

// ------------------------------
// �֐��v���g�^�C�v
// ------------------------------

#ifndef __ASSEMBLER_H__
#define __ASSEMBLER_H__

#include "typedef.h"
#include "define.h"

namespace Simulator {

	using namespace std;

	public class Assembler
	{
	public:
		Assembler();
		~Assembler();

		int asmmain(int argc, char *fn);
		char*	get_infilename();
		char*	get_outfilename();

	private:
		int toh_assemble(FILE *fp);
		int toh_analysis(char *mnemonic);
		int toh_output_file(FILE *fp, int start, int end, unsigned short *mem);
		unsigned short* toh_mc_buff_add(void);

		//�x�[�X�A�h���X
		unsigned int g_basis_addr;

		//�}�V���R�[�h�o�b�t�@
		unsigned short mc_buff[MACHINE_CODE_LIST_MAX];
		int	pc;
		int start_mc;

		//���x�����ԃf�[�^
		ST_LABEL_ELEMENT label_def_list[LABEL_LIST_MAX];
		ST_LABEL_ELEMENT label_ref_list[LABEL_LIST_MAX];

		char m_infilename[1024];
		char m_outfilename[1024];

	};

};

#endif  //__ASSEMBLER_H__
