/*
 * ȭ�ϸ� : my_assembler_20162449.c
 * ��  �� : �� ���α׷��� SIC/XE �ӽ��� ���� ������ Assembler ���α׷��� ���η�ƾ����,
 * �Էµ� ������ �ڵ� ��, ��ɾ �ش��ϴ� OPCODE�� ã�� ����Ѵ�.
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h >
#include <stdlib.h >
#include <string.h >
#include <fcntl.h >
#include "my_assembler_20162449.h"
 /* ----------------------------------------------------------------------------------
  * ���� : ����ڷ� ���� ����� ������ �޾Ƽ� ��ɾ��� OPCODE�� ã�� ����Ѵ�.
  * �Ű� : ���� ����, ����� ����
  * ��ȯ : ���� = 0, ���� = < 0
  * ���� : ���� ����� ���α׷��� ����Ʈ ������ �����ϴ� ��ƾ�� ������ �ʾҴ�.
  *		   ���� �߰������� �������� �ʴ´�.
  * ----------------------------------------------------------------------------------
  */
int main(int args, char* arg[])
{
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: ���α׷� �ʱ�ȭ�� ���� �߽��ϴ�.\n");
		return -1;
	}
	if (assem_pass1() < 0)
	{
		printf("assem_pass1: �н�1 �������� �����Ͽ����ϴ�.  \n");
		return -1;
	}
	make_symtab_output("symtab_20162449");
	make_literaltab_output("literaltab_20162449");
	if (assem_pass2() < 0)
	{
		printf(" assem_pass2: �н�2 �������� �����Ͽ����ϴ�.  \n");
		return -1;
	}
	make_objectcode_output("output_20162449");
	return 0;
}
/* ----------------------------------------------------------------------------------
 * ���� : ���α׷� �ʱ�ȭ�� ���� �ڷᱸ�� ���� �� ������ �д� �Լ��̴�.
 * �Ű� : ����
 * ��ȯ : �������� = 0 , ���� �߻� = -1
 * ���� : ������ ��ɾ� ���̺��� ���ο� �������� �ʰ� ������ �����ϰ� �ϱ�
 *		   ���ؼ� ���� ������ �����Ͽ� ���α׷� �ʱ�ȭ�� ���� ������ �о� �� �� �ֵ���
 *		   �����Ͽ���.
 * ----------------------------------------------------------------------------------
 */
int init_my_assembler(void)
{
	printf("*********************init_my_assembler********************\n\n");
	int result;
	if ((result = init_inst_file("inst.data")) < 0)
		return -1;
	if ((result = init_input_file("input.txt")) < 0)
		return -1;
	printf("\n*********************************************************\n");
	return result;
}
/* ----------------------------------------------------------------------------------
 * ���� : �ӽ��� ���� ��� �ڵ��� ������ �о� ���� ��� ���̺�(inst_table)��
 *        �����ϴ� �Լ��̴�.
 * �Ű� : ���� ��� ����
 * ��ȯ : �������� = 0 , ���� < 0
 * ���� : ���� ������� ������ �����Ӱ� �����Ѵ�. ���ô� ������ ����.
 *
 *	===============================================================================
 *		   | �̸� | ���� | ���� �ڵ� | ���۷����� ���� | NULL|
 *	===============================================================================
 * ----------------------------------------------------------------------------------
 */
int init_inst_file(char* inst_file)
{
	printf("--------------init_inst_file----------------------\n");
	FILE* file;
	int err = 0;
	int i = 0;
	int num;
	char buffer[100];
	char* ptr;
	if ((file = fopen(inst_file, "r")) < 0) {
		return -1;
	}
	while (feof(file) == 0) {
		inst_table[i] = malloc(sizeof(inst));
		fgets(buffer, sizeof(buffer), file);
		check_Enter(buffer);
		ptr = strtok(buffer, ",");			//strtok buffer with ","
		strcpy(inst_table[i]->str, ptr);
		ptr = strtok(NULL, ",");
		if (strchr(ptr, '/') != NULL) {
			ptr[strlen(ptr) - 2] = '\0';
			inst_table[i]->format = atoi(ptr); // save first letter in inst_table format , ex) 3/4 -> 3
		}
		else {
			inst_table[i]->format = atoi(ptr);
		}
		ptr = strtok(NULL, ",");
		num = StringToHexNum(ptr);			//StringToHexNum 
		if (num < 0) {
			return -1;
		}
		inst_table[i]->op = num;
		ptr = strtok(NULL, ",");
		inst_table[i]->ops = atoi(ptr);
		printf("%d . %s/%d/%x/%d\n", i + 1, inst_table[i]->str, inst_table[i]->format, inst_table[i]->op, inst_table[i]->ops);
		i++;
	}
	inst_index = i;
	fclose(file);
	/* add your code here */
	printf("sucess init_inst_file\n");

	printf("--------------------------------------------------\n");
	return err;
}
/*------------------------------------------------------------------------------------
 * ���� : ���ڿ������� 16������ �ǹ̸� ��� �ֱ� ������ �̸� �����ϱ� ���ؼ��� ���ڿ��� ���������� �ٲ�����Ѵ�.
		  ex ) 1C -> 1*16 + 12
 * �Ű� : ���ڿ�
 * ��ȯ : ����
 -------------------------------------------------------------------------------------*/
int StringToHexNum(char* input) {
	int i;
	unsigned char num = 0;
	for (i = 0; i < 2; i++) {
		if (input[i] >= '0' && input[i] <= '9')
			num = num * 16 + input[i] - '0';	// because num is hex num, *16 .
		else if (input[i] >= 'A' && input[i] <= 'Z')
			num = num * 16 + input[i] - 'A' + 10;
		else
			return -1;
	}
	return num;
}
/* ----------------------------------------------------------------------------------
 * ���� : ����� �� �ҽ��ڵ带 �о� �ҽ��ڵ� ���̺�(input_data)�� �����ϴ� �Լ��̴�.
 * �Ű� : ������� �ҽ����ϸ�
 * ��ȯ : �������� = 0 , ���� < 0
 * ���� : ���δ����� �����Ѵ�.
 *
 * ----------------------------------------------------------------------------------
 */
int init_input_file(char* input_file)
{
	printf("--------------init_input_file----------------------\n");
	FILE* file;
	int err = 0;
	int i = 0;
	char buffer[100]; // for one line buffer
	if ((file = fopen(input_file, "r")) < 0) {
		return -1;
	}
	while (feof(file) == 0) {
		input_data[i] = malloc(sizeof(char) * 100);
		fgets(buffer, sizeof(buffer), file);
		strcpy(input_data[i], buffer);
		//	printf("%s",input_data[i]);
		i++;
	}
	/* add your code here */
	line_num = i - 1;
	printf("sucess init_input_file\n");
	printf("--------------------------------------------------\n");
	return err;
}
/* ----------------------------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �н�1������ �����ϴ� �Լ��̴�.
*		   �н�1������..
*		   1. ���α׷� �ҽ��� ��ĵ�Ͽ� �ش��ϴ� ��ū������ �и��Ͽ� ���α׷� ���κ� ��ū
*		   ���̺��� �����Ѵ�.
*
* �Ű� : ����
* ��ȯ : ���� ���� = 0 , ���� = < 0
* ���� : ���� �ʱ� ���������� ������ ���� �˻縦 ���� �ʰ� �Ѿ �����̴�.
*	  ���� ������ ���� �˻� ��ƾ�� �߰��ؾ� �Ѵ�.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
	printf("\n******************assem_pass1*********************\n");
	int i = 0;
	/* add your code here */
	printf("-------------- token_parsing----------------------\n");
	while (i < line_num) {
		if (token_parsing(input_data[i]) < 0)
			return -1;
		i++;
	}
	printf("token_parsing sucess\n");
	line_num = token_line;
	printf("---------------------------------------------------\n");
	input_locctr_token_table("intermediate");
	printf("****************************************************\n");
	return 0;
	/* input_data�� ���ڿ��� ���پ� �Է� �޾Ƽ�
	 * token_parsing()�� ȣ���Ͽ� token_unit�� ����
	 */
}
/* ----------------------------------------------------------------------------------
 * ���� : �ҽ� �ڵ带 �о�� ��ū������ �м��ϰ� ��ū ���̺��� �ۼ��ϴ� �Լ��̴�.
 *        �н� 1�� ���� ȣ��ȴ�.
 * �Ű� : �Ľ��� ���ϴ� ���ڿ�
 * ��ȯ : �������� = 0 , ���� < 0
 * ���� : my_assembler ���α׷������� ���δ����� ��ū �� ������Ʈ ������ �ϰ� �ִ�.
 * ----------------------------------------------------------------------------------
 */
int token_parsing(char* str)
{
	//memory init--------------------------------------------------------------------
	token_table[token_line] = malloc(sizeof(token));
	token_table[token_line]->label = malloc(sizeof(char) * 20);
	token_table[token_line]->operator =malloc(sizeof(char) * 20);
	//-------------------------------------------------------------------------------
	char* label = NULL, * operator =NULL, * operand = NULL, * comment = NULL;
	char* operand1 = NULL, * operand2 = NULL, * operand3 = NULL;
	char** fields[] = { &label, &operator, &operand, &comment, NULL };
	char** operand_fields[] = { &operand1, &operand2, &operand3 };
	// �ּ� ���͸�
	if (str[0] == '.')
		return 0;
	// Tab�� �����ڷ� �Ͽ� �� �ʵ� �и�
	for (int i = 0; fields[i] != NULL; i++) {
		*fields[i] = str;
		if ((str = strpbrk(str, "\t\n")) == NULL)
			break;
		*str++ = '\0'; // �߰��� NULL ���ڸ� �־� ���ڿ� �и�
	}
	for (int i = 0; operand_fields[i] != NULL; i++) {
		*operand_fields[i] = operand;
		if ((operand = strpbrk(operand, ",")) == NULL)
			break;
		*operand++ = '\0'; // �߰��� NULL ���ڸ� �־� ���ڿ� �и�
	}
	strcpy(token_table[token_line]->label, label);
	strcpy(token_table[token_line]->operator ,operator);
	if (operand1 != '\0')
		strcpy(token_table[token_line]->operand[0], operand1);
	else
		strcpy(token_table[token_line]->operand[0], "");
	if (operand2 != '\0')
		strcpy(token_table[token_line]->operand[1], operand2);
	else
		strcpy(token_table[token_line]->operand[1], "");
	if (operand3 != '\0')
		strcpy(token_table[token_line]->operand[2], operand3);
	else
		strcpy(token_table[token_line]->operand[2], "");
	if (comment != '\0')
		strcpy(token_table[token_line]->comment, comment);
	else
		strcpy(token_table[token_line]->comment, "");
	token_line++;
	return 0;
}
/* ----------------------------------------------------------------------------------
* ���� : ������ token_table�� locctr�� �߰��ϴ� �Լ�
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* -----------------------------------------------------------------------------------
*/
void input_locctr_token_table(void)
{
	printf("--------------input_locctr_token_table----------------------\n");
	search_ltorg_index = 0;
	int temp_index1, temp_index2;
	int i = 0;
	int index;
	csec_num = 0;
	locctr = 0;
	while (i < line_num) {// �����ּҸ� ã������ START ���� operand ���� �����Ѵ�.
		if (strcmp(token_table[i]->operator,"START") == 0) {
			locctr = atoi(token_table[i]->operand[0]);
			break;
		}
		i++;
	}
	strloc = locctr;
	i = 0;
	while (i < line_num) {
		if (strcmp(token_table[i]->operator,"CSECT") == 0)
			csec_num++;
		token_table[i]->csect = csec_num;
		token_table[i]->locctr = locctr;
		index = search_opcode(token_table[i]->operator);
		if (index < 0) {
			if (strcmp(token_table[i]->operator,"RESW") == 0) {
				locctr += (3 * atoi(token_table[i]->operand[0]));  //word �� 1���� 3����Ʈ!
			}
			else if (strcmp(token_table[i]->operator,"RESB") == 0) {
				locctr += atoi(token_table[i]->operand[0]);
			}
			else if (strcmp(token_table[i]->operator,"CSECT") == 0) { // ���ο� �����ƾ �ǹ��ϱ� ������ 0���� locctr �ʱ�ȭ
				token_table[i]->locctr = 0;
				locctr = 0;
			}
			else if (strcmp(token_table[i]->operator,"LTORG") == 0) { //LRORG �� ã���� 
				locctr += (strlen(token_table[Search_LTORG(i)]->operand[0]) - 4); // Search_LTORG �Լ��� �̵��Ͽ� search_ltorg_index ���� ���� index ������ '='�̵� operand�� ã�´�.
				search_ltorg_index = i;
			}
			else if (strcmp(token_table[i]->operator,"BYTE") == 0) {
				if (token_table[i]->operand[0][0] == 'X')
					locctr += (strlen(token_table[i]->operand[0]) - 3) / 2;
				//if(token_table[i]->operand[0][0] == '') // �̰��� 'X' ���� �ٸ� ������ ������ �ȴ�.
			}
			else if (strchr(token_table[i]->operand[0], '-') != NULL && strcmp(token_table[i]->operator,"EQU") == 0) { // BUFFER -BUFEND ���� ��쿡���� ���� ó��
				char* ptr = strtok(token_table[i]->operand[0], "-");
				temp_index1 = Search_Label(ptr);
				ptr = strtok(NULL, "-");
				temp_index2 = Search_Label(ptr);
				token_table[i]->locctr = token_table[temp_index1]->locctr - token_table[temp_index2]->locctr;
			}
			else {
				locctr += 0;
			}
			printf("locctr : %x \t%s\t%s\t%s\t%s\t%d\n", token_table[i]->locctr, token_table[i]->label, token_table[i]->operator,token_table[i]->operand[0], token_table[i]->operand[1], token_table[i]->csect);
			i++;
			continue;
		}
		if (token_table[i]->operator[0] == '+') {
			locctr += 4;
		}
		else
			locctr += inst_table[index]->format;
		printf("locctr : %x \t%s\t%s\t%s\t%s\t%d\n", token_table[i]->locctr, token_table[i]->label, token_table[i]->operator,token_table[i]->operand[0], token_table[i]->operand[1], token_table[i]->csect);

		i++;
	}
	printf("sucess\n");
	printf("---------------------------------------------------\n");
}
/*--------------------------------------------------------------------------------------------------------
 *���� : (�߰�) LTORG �� ������ �� �� ���� '='���� �����ϴ� operand�� ã�´�
 *�Ű� : void
 *��ȯ : �� token_table�� index
 --------------------------------------------------------------------------------------------------------*/
int Search_LTORG(int index) {
	for (int i = search_ltorg_index; i < index; i++) {
		if (token_table[i]->operand[0][0] == '=') // =C'EOF' ��� �Ҷ� operand[0][0]= '=' �� �ǰ� �̶��� index�� ��ȯ.
			return i;
	}
}
/*------------------------------------------------------------------
* ���� : �־��� label�� �����ϴ� token_table�� inst_table�� op�� ���Ͽ� �����ϸ� �� op�� ��ȯ,
		�̶� +jsub ���� ��찡 ������ �� �����Ƿ� ���ڿ� ù��° ���ڸ� ���ְ� �ٽ� ���� ���� �ݺ�.
* �Ű� : ���ڿ�
* ��ȯ : �������� = ���� ���̺� �ε���, ���� < 0
-------------------------------------------------------------------*/
int Search_Label(char* label) {
	int i;
	for (i = 0; i < line_num; i++) {
		if (strcmp(token_table[i]->label, label) == 0) {
			return i;
		}
	}
	return -1;
}
/*------------------------------------------------------------------
* ���� : �־��� input ���ڿ��� inst_table�� op�� ���Ͽ� �����ϸ� �� op�� ��ȯ,
		�̶� +jsub ���� ��찡 ������ �� �����Ƿ� ���ڿ� ù��° ���ڸ� ���ְ� �ٽ� ���� ���� �ݺ�.
* �Ű� : ���ڿ�
* ��ȯ : �������� = ���� ���̺� �ε���, ���� < 0
-------------------------------------------------------------------*/
int search_opcode(char* input) {
	int i, j;
	char* tmp_input = malloc(sizeof(input));
	strcpy(tmp_input, input);
	for (i = 0; i < inst_index; i++) {
		if (strcmp(inst_table[i]->str, input) == 0)
			return i;
	}
	for (j = 1; tmp_input[j]; j++) {
		tmp_input[j - 1] = tmp_input[j];
	}//ù ���� ����
	tmp_input[j - 1] = '\0';
	for (i = 0; i < inst_index; i++) {
		if (strcmp(inst_table[i]->str, tmp_input) == 0)
			return i;
	}
	return -1;
}
/*------------------------------------------------------------------
* ���� : fgets() �Լ��� ���� �� ���ε鿡 �������� ���๮�ڰ� ���鼭
		�� ���̺�� �Ľ̵Ǵ� �������� ���๮�ڵ��� ���� ��
		���ϴ� output.txt�� ������� ���ϱ淡 ���๮�ڸ� ���ִ� �Լ��� ��������ϴ�.
* �Ű� : ���ڿ�
* ��ȯ : ����
-------------------------------------------------------------------*/
void check_Enter(char* input) {
	int i = 0;
	for (i = 0; input[i] != 0; i++) {
		if (input[i] == '\n') {
			input[i] = 0;
			break;
		}
	}
}
/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ SYMBOL�� �ּҰ��� ����� TABLE�̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_symtab_output(char* file_name)
{
	symbal_line = 0;
	FILE* fp;
	fp = fopen(file_name, "w");
	int i = 0;
	int cset_num = 0;
	while (i < line_num) {
		if (strcmp(token_table[i]->operator,"CSECT") == 0)
			cset_num++;
		if (token_table[i]->label[0] != '\0') {
			sym_table[symbal_line].symbol = malloc(sizeof(symbol) * 20);
			strcpy(sym_table[symbal_line].symbol, token_table[i]->label); printf("%s\t", sym_table[symbal_line].symbol);
			sym_table[symbal_line].addr = token_table[i]->locctr; printf("%x\t", sym_table[symbal_line].addr);
			sym_table[symbal_line].csec = cset_num; printf("%x\n", sym_table[symbal_line].csec);
			fprintf(fp, "%s\t", sym_table[symbal_line].symbol);
			fprintf(fp, "%x\n", sym_table[symbal_line].addr);
			symbal_line++;
		}
		i++;
	}
	printf("\nfin symtable\n");
	fclose(fp);
}
/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ LITERAL�� �ּҰ��� ����� TABLE�̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_literaltab_output(char* file_name)
{
	FILE* fp;
	fp = fopen(file_name, "w");
	char temp[100];
	int i = 0;
	int flag = 0;
	literal_line = 0;
	while (i < line_num) {
		literal_table[i].literal = malloc(sizeof(char) * 20);
		if (token_table[i]->operand[0][0] == '=') {
			for (int j = i; j < line_num; j++) {// literal �� index ���� ~ ������ LTORG�� operator�� �������� �˻��Ѵ�.
				if (strcmp(token_table[j]->operator,"LTORG") == 0) {
					if (check_literal_table(token_table[i]->operand[0]) < 0) {//literal_table �ߺ��˻�
						strcpy(literal_table[literal_line].literal, token_table[i]->operand[0]);
						printf("%s\t", literal_table[literal_line].literal);
						literal_table[literal_line].addr = token_table[j]->locctr;
						printf("%d\n", literal_table[literal_line].addr);
						fprintf(fp, "%s\t%x\n", literal_table[literal_line].literal, literal_table[literal_line].addr);
						strcpy(token_table[j]->operator,"BYTE");
						strcpy(token_table[j]->operand[0], literal_table[literal_line].literal);
						literal_line++;
						flag = 1;
						break;
					}
				}
			}
			if (flag == 0) { // '='�� ���Ե� operand �ؿ� LTORG�� ���ٴ� ��!
				if (check_literal_table(token_table[i]->operand[0]) < 0) {//literal_table �ߺ��˻�
					strcpy(literal_table[literal_line].literal, token_table[i]->operand[0]);
					printf("%s\t", literal_table[literal_line].literal);
					literal_table[literal_line].addr = locctr;
					printf("%d\n", literal_table[literal_line].addr);
					locctr += (strlen(token_table[i]->operand[0]) - 3) / 2;
					fprintf(fp, "%s\t%x\n", literal_table[literal_line].literal, literal_table[literal_line].addr);
					strcpy(token_table[line_num - 1]->operator,"BYTE");
					strcpy(token_table[line_num - 1]->operand[0], literal_table[literal_line].literal);
					literal_line++;
				}
			}
		}
		flag = 0;
		i++;
	}
	printf("\nfin literaltab\n");
	fclose(fp);
	/* add your code here */
}
/* ----------------------------------------------------------------------------------
 * ���� : literal table�� �ߺ��� ���� ���� �ʵ��� Ȯ���ϴ� �Լ��̴�.
 * �Ű� : ���ڿ� (oeprand[0])
 * ��ȯ : �̹� �����ϸ� 1 ���ٸ� -1
 * ----------------------------------------------------------------------------------
 */
int check_literal_table(char* str) {
	int i;
	for (i = 0; i < literal_line; i++) {
		if (strcmp(literal_table[i].literal, str) == 0) {
			return 1;
		}
	}
	return -1;
}
/* ----------------------------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �ڵ�� �ٲٱ� ���� �н�2 ������ �����ϴ� �Լ��̴�.
*		   �н� 2������ ���α׷��� ����� �ٲٴ� �۾��� ���� ������ ����ȴ�.
*		   ������ ���� �۾��� ����Ǿ� ����.
*			1. ������ �ش� ����� ��ɾ ����� �ٲٴ� �۾��� �����Ѵ�.
*			2. ����� ��ɾ���� opcode_table �����Ѵ�.
* �Ű� : ����
* ��ȯ : �������� = 0, �����߻� = < 0
* ���� :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{
	int i = 0;

	while (i < line_num) {
		printf("locctr : %x \t%s\t%s\t%s\t%s\n", token_table[i]->locctr, token_table[i]->label, token_table[i]->operator,token_table[i]->operand[0], token_table[i]->operand[1]);
		i++;
	}
	i = 0;
	int j;
	opcode_table_line_num = 0;
	csec_num = 0;
	printf("\n******************assem_pass2*********************\n");
	int temp = 0;
	int index = 0;
	int temp_index;
	int temp_hex = 0;
	char temp_char[100];
	csec_num = 0;
	while (i < line_num) {
		if (strcmp(token_table[i]->operator,"CSECT") == 0)
			csec_num++;
		token_table[i]->nixbpe = 0;
		index = search_opcode(token_table[i]->operator);
		if (index >= 0) { // operator �� inst_table �� �����Ҷ�
			if (inst_table[index]->format == 3) {
				if (token_table[i]->operand[0][0] == '@') {//immidiate
					token_table[i]->nixbpe += 1;
					token_table[i]->nixbpe = token_table[i]->nixbpe << 5; // n:1 i:0 x:0 p:0 e:0
					token_table[i]->nixbpe += 2; // n:1 i:0 x:0 p:1 e:0 indirect �� pc relative ���
					temp = inst_table[index]->op; temp = temp << 4;
					temp += token_table[i]->nixbpe;
					temp = temp << 12;
					opcode_table[opcode_table_line_num].opcode = temp;
					opcode_table[opcode_table_line_num].format = 3;
					opcode_table[opcode_table_line_num].addr = token_table[i]->locctr;
					opcode_table[opcode_table_line_num].csec_num = token_table[i]->csect;
					opcode_table_line_num++;
					i++;
					continue;
				}
				else if (token_table[i]->operand[0][0] == '#') { //indirect
					token_table[i]->nixbpe += 1;
					token_table[i]->nixbpe = token_table[i]->nixbpe << 4;
					temp = inst_table[index]->op; temp = temp << 4;
					temp += token_table[i]->nixbpe;
					temp = temp << 12;
					strcpy(temp_char, token_table[i]->operand[0]);
					//printf("!!%s\n", temp_char);
					temp_char[0] = '0'; // ���ڿ��� �Ǿ��ִ� " # '�ּ�' " ���� �ּҸ� �Ľ��ϱ� ���� '#'�� '0'���� ġȯ��
					//��ĭ
					temp += atoi(temp_char);
					opcode_table[opcode_table_line_num].opcode = temp;
					opcode_table[opcode_table_line_num].format = 3;
					opcode_table[opcode_table_line_num].addr = token_table[i]->locctr;
					opcode_table[opcode_table_line_num].csec_num = token_table[i]->csect;
					opcode_table_line_num++;
					i++;
					continue;
				}

				else {
					token_table[i]->nixbpe += 3;
					token_table[i]->nixbpe = token_table[i]->nixbpe << 4;
				}
				if (token_table[i]->operand[1][0] == 'X') {
					token_table[i]->nixbpe += 8;
				}// x �� �������϶� 
				if (token_table[i]->operator[0] == '+') { //4���� �϶�
					token_table[i]->nixbpe += 1;
					temp = inst_table[index]->op; temp = temp << 4;
					temp += token_table[i]->nixbpe;
					//	printf("%x\t", temp);
					temp = temp << 20;
					opcode_table[opcode_table_line_num].opcode = temp;
					opcode_table[opcode_table_line_num].format = 4;
					opcode_table[opcode_table_line_num].addr = token_table[i]->locctr;
					opcode_table[opcode_table_line_num].csec_num = token_table[i]->csect;
					opcode_table_line_num++;
					i++;
					continue;
				}
				else if (strcmp(token_table[i]->operator,"RSUB") == 0) {
					temp = inst_table[index]->op;
					temp = temp << 4;// op�ڵ带 4�� �������� ����Ʈ�Ͽ� nixbpe�� ���� �ڸ��� �����.
					temp += token_table[i]->nixbpe;
					temp = temp << 12;
					opcode_table[opcode_table_line_num].opcode = temp;
					opcode_table[opcode_table_line_num].format = 3;
					opcode_table[opcode_table_line_num].addr = token_table[i]->locctr;
					opcode_table[opcode_table_line_num].csec_num = token_table[i]->csect;
					opcode_table_line_num++;
					i++;
					continue;
				}
				else {
					token_table[i]->nixbpe += 2;
				}
			here:
				temp = inst_table[index]->op;
				temp = temp << 4;// op�ڵ带 4�� �������� ����Ʈ�Ͽ� nixbpe�� ���� �ڸ��� �����.
				temp += token_table[i]->nixbpe;
				temp = temp << 12;// opcode �� nixbpe �� ���Ͽ��� ���� �ڿ� 16���� 000�� �ֱ� ���� �������� 12�� ����Ʈ
				//printf("%.6x\n", temp);
				//printf("%s :%x,  %x \n", token_table[i]->operand[0],addr_found(token_table[i]->operand[0]), token_table[i + 1]->locctr);
				if (addr_found(token_table[i]->operand[0], csec_num) - token_table[i + 1]->locctr >= 0) {
					//printf("%x  %x\t", addr_found(token_table[i]->operand[0]), token_table[i + 1]->locctr);
					temp = temp + addr_found(token_table[i]->operand[0], csec_num) - token_table[i + 1]->locctr;
				}
				else {//target�ּ� - ���� program counter <0 �̶�� ������ �̿��Ͽ� Ǭ��
					//printf("%s  %x  %x\t", token_table[i]->operand[0], addr_found(token_table[i]->operand[0]), token_table[i + 1]->locctr);
					temp += (4096 - token_table[i + 1]->locctr + addr_found(token_table[i]->operand[0], csec_num));
				}
				opcode_table[opcode_table_line_num].opcode = temp;
				opcode_table[opcode_table_line_num].format = 3;
				opcode_table[opcode_table_line_num].addr = token_table[i]->locctr;
				opcode_table[opcode_table_line_num].csec_num = token_table[i]->csect;
				opcode_table_line_num++;
			}
			else if (inst_table[index]->format == 2) {//2�����϶� 
				temp = inst_table[index]->op << 8; //8�� ����Ʈ �ߴٴ� ���� 16���� B4 ��� B400�� ���� �ǹ��Ѵ�.
				if (strcmp(token_table[i]->operand[0], "X") == 0) {
					temp += (16 * 1); // 16�� ���ϴ� ������ 2��° �ڸ��� �־�� �ϱ� �����̴�!
				}
				else if (strcmp(token_table[i]->operand[0], "A") == 0) {
					temp += (16 * 0);
				}
				else if (strcmp(token_table[i]->operand[0], "T") == 0) {
					temp += (16 * 5);
				}
				else if (strcmp(token_table[i]->operand[0], "S") == 0) {
					temp += (16 * 4);
				}

				if (strcmp(token_table[i]->operand[1], "X") == 0) {
					temp += 1;
				}
				else if (strcmp(token_table[i]->operand[1], "A") == 0) {
					temp += 0;
				}
				else if (strcmp(token_table[i]->operand[1], "T") == 0) {
					temp += 5;
				}
				else if (strcmp(token_table[i]->operand[1], "S") == 0) {
					temp += 4;
				}
				//printf("%x\n", temp);
				opcode_table[opcode_table_line_num].opcode = temp;
				opcode_table[opcode_table_line_num].format = 2;
				opcode_table[opcode_table_line_num].addr = token_table[i]->locctr;
				opcode_table[opcode_table_line_num].csec_num = token_table[i]->csect;
				opcode_table_line_num++;
			}
		}
		else { //�ش� operator�� inst_table�� �������� ���� ��
			if (strcmp(token_table[i]->operator,"BYTE") == 0) {
				if (token_table[i]->operand[0][0] != '=') { // X'F1' �� ���� ���� ó��
					strcpy(temp_char, token_table[i]->operand[0]);
					//printf("%s\n", temp_char);
					temp_char[strlen(temp_char) - 1] = '\0';
					temp_char[0] = '0';
					temp_char[1] = '0';//temp_char = "00F1"
					//printf("%d\n", strtol(temp_char, NULL, 16));
					opcode_table[opcode_table_line_num].opcode = strtol(temp_char, NULL, 16);
					opcode_table[opcode_table_line_num].format = 0;
					opcode_table[opcode_table_line_num].addr = token_table[i]->locctr;
					opcode_table[opcode_table_line_num].csec_num = token_table[i]->csect;
					//	printf("BYTE X %x  %d\n", opcode_table[opcode_table_line_num].opcode, opcode_table[opcode_table_line_num].format);
					opcode_table_line_num++;
				}
				if (token_table[i]->operand[0][1] == 'X') { // =X'F1'�� ���� ����ó��
					strcpy(temp_char, token_table[i]->operand[0]);
					//printf("%s\n", temp_char);
					temp_char[strlen(temp_char) - 1] = '\0';
					temp_char[0] = '0';
					temp_char[1] = '0';
					temp_char[2] = '0'; // temp_char = "000F1"
					//printf("%d\n", strtol(temp_char, NULL, 16));
					opcode_table[opcode_table_line_num].opcode = strtol(temp_char, NULL, 16);
					opcode_table[opcode_table_line_num].format = 0;
					opcode_table[opcode_table_line_num].addr = token_table[i]->locctr;
					opcode_table[opcode_table_line_num].csec_num = token_table[i]->csect;
					//	printf("BYTE X %x  %d\n", opcode_table[opcode_table_line_num].opcode, opcode_table[opcode_table_line_num].format);
					opcode_table_line_num++;
				}
				if (token_table[i]->operand[0][1] == 'C') { // =C' ' �϶�
				//	printf("/%s\n", token_table[i]->operand[0]);
					strcpy(temp_char, token_table[i]->operand[0]);

					for (int k = 0; k < strlen(temp_char) - 4; k++) {// tamp_char�� "=C'EOF'" �϶� strlen (tamp_char)-4�� 3, �� EOF �� strlen�� ���´�.
						temp_hex = temp_hex << (8); //16���� �󿡼� ��ĭ ������ �ű�� ���Ͽ�
						temp_hex += temp_char[k + 3]; //temp_char[3] = 'E'= 45 temp_char[4] = 'O' = 4F temp_char[5] ='F'=46
						//000000 + 000045 = 000045, 000045 << 8 = 004500 , 004500 + 00004F = 00454F, 00454F << 8 = 454F00 , 454F00 + 000046 = 454F46.
					}
					opcode_table[opcode_table_line_num].opcode = temp_hex;
					opcode_table[opcode_table_line_num].format = 0;
					opcode_table[opcode_table_line_num].addr = token_table[i]->locctr;
					opcode_table[opcode_table_line_num].csec_num = token_table[i]->csect;
					//	printf("BYTE C %x  %d\n", opcode_table[opcode_table_line_num].opcode, opcode_table[opcode_table_line_num].format);
					opcode_table_line_num++;
				}
			}
			else if (strcmp(token_table[i]->operator,"WORD") == 0) {
				temp = 0;
				opcode_table[opcode_table_line_num].opcode = temp;
				opcode_table[opcode_table_line_num].format = 0;
				opcode_table[opcode_table_line_num].addr = token_table[i]->locctr;
				opcode_table[opcode_table_line_num].csec_num = token_table[i]->csect;
				opcode_table_line_num++;
			}

			//	printf("0\n");
		}
		i++;
	}

	//printf("%x\n",addr_found("CLOOP"));
	printf("-----\n");
	for (int i = 0; i < opcode_table_line_num; i++) {
		printf("%.6x  %x  %d\n", opcode_table[i].opcode, opcode_table[i].addr, opcode_table[i].csec_num);
	}
	printf("****************************************************\n");
	return 0;
}
/* ----------------------------------------------------------------------------------
 * ���� : �־��� csec_num�� ���ڿ��� ������ �ɺ����̺�� ���ͷ����̺��� �����鼭 �����ϴ� �� ã�� �Լ��̴�.
 * �Ű� : ���ڿ� �����ƾ ��ȣ
 * ��ȯ : �ּ� ��ȯ
 * ----------------------------------------------------------------------------------
 */
int addr_found(char* str, int csec_num) {
	int i;
	int index;
	int count = 0;
	for (i = 0; i < symbal_line; i++) {
		if (strcmp(str, sym_table[i].symbol) == 0 && sym_table[i].csec == csec_num) {
			return sym_table[i].addr;
		}
	}
	for (i = 0; i < literal_line; i++) {
		if (strcmp(str, literal_table[i].literal) == 0) {
			//printf("in literal_table : ");
			return literal_table[i].addr;
		}
	}
	return 0;
}
/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ object code (������Ʈ 1��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char* file_name)
{
	FILE* fp;
	fp = fopen(file_name, "w");
	int j = 0;
	int i = 0;
	int index = 0;
	int count = 0;
	char buffer[5000] = { 0, };
	int temp_addr = 0;
	char temp_buffer[5000] = { 0, };
	for (i = 0; i <= csec_num; i++) {
		for (j = 0; j < symbal_line; j++) {
			if (sym_table[j].csec == i) {
				fprintf(fp, "H");
				fprintf(fp, "%s", sym_table[j].symbol);
				fprintf(fp, "%.6x", sym_table[j].addr);
				fprintf(fp, "%.6x", found_bigaddr(i));
				fprintf(fp, "\n");
				if (i == 0) {
					fprintf(fp, "D");
					while (index < line_num) {
						if (strcmp(token_table[index]->operator,"EXTDEF") == 0 && token_table[index]->csect == i) {
							fprintf(fp, "%s", token_table[index]->operand[0]);
							fprintf(fp, "%.6x", addr_found(token_table[index]->operand[0], i));
							if (token_table[index]->operand[1][0] != NULL) {
								fprintf(fp, "%s", token_table[index]->operand[1]);
								fprintf(fp, "%.6x", addr_found(token_table[index]->operand[1], i));
							}
							if (token_table[index]->operand[2][0] != NULL) {
								fprintf(fp, "%s", token_table[index]->operand[2]);
								fprintf(fp, "%.6x", addr_found(token_table[index]->operand[2], i));
							}
						}
						index++;
					}
					fprintf(fp, "\n");
				}
				fprintf(fp, "R");
				index = 0;
				while (index < line_num) {
					if (strcmp(token_table[index]->operator,"EXTREF") == 0 && token_table[index]->csect == i) {
						fprintf(fp, "%s", token_table[index]->operand[0]);
						if (token_table[index]->operand[1][0] != NULL) {
							fprintf(fp, "%s", token_table[index]->operand[1]);
						}
						if (token_table[index]->operand[2][0] != NULL) {
							fprintf(fp, "%s", token_table[index]->operand[2]);
						}
					}
					index++;
				}
				fprintf(fp, "\n");
				for (int k = 0; k < opcode_table_line_num; k++) {
					if (opcode_table[k].csec_num == i) {
						if (opcode_table[k].format == 2) {
							sprintf(temp_buffer, "%.4x", opcode_table[k].opcode);
							strcat(buffer, temp_buffer);
							memset(temp_buffer, 0, sizeof(temp_buffer));
						}
						else if (opcode_table[k].format == 4) {
							sprintf(temp_buffer, "%.8x", opcode_table[k].opcode);
							strcat(buffer, temp_buffer);
							memset(temp_buffer, 0, sizeof(temp_buffer));
						}
						else {
							if (opcode_table[k].opcode == 0) {
								sprintf(temp_buffer, "%.6x", opcode_table[k].opcode);
							}
							else if (opcode_table[k].opcode < 256) {
								sprintf(temp_buffer, "%.2x", opcode_table[k].opcode);
							}
							else
								sprintf(temp_buffer, "%.6x", opcode_table[k].opcode);
							strcat(buffer, temp_buffer);
							memset(temp_buffer, 0, sizeof(temp_buffer));
						}
					}
					if (strlen(buffer) > 54) {
						strcat(buffer, "\n");
						fprintf(fp, "T");
						fprintf(fp, "%.6x ", temp_addr);
						temp_addr = opcode_table[k + 1].addr;
						fprintf(fp, "%.2x ", strlen(buffer) / 2);
						fprintf(fp, "%s", buffer);
						memset(buffer, 0, sizeof(buffer));
					}
				}
				if (strlen(buffer) / 2 > 0) {
					fprintf(fp, "T");
					fprintf(fp, "%.6x ", temp_addr);
					fprintf(fp, "%.2x ", strlen(buffer) / 2);
					fprintf(fp, "%s", buffer);
					memset(buffer, 0, sizeof(buffer));
					fprintf(fp, "\n");
				}
				temp_addr = 0;
				break;
			}
		}fprintf(fp, "\n");
	}

	fclose(fp);
	/* add your code here */
}
/* ----------------------------------------------------------------------------------
 * ���� : ���� csec_num �߿��� ���� ū �ּҸ� ã������ �Լ��̴�.
 *		  object������ ���鶧 ������ �����ƾ�� ũ�⸦ �����Ѵ�.
 * �Ű� : �����ƾ ��ȣ
 * ��ȯ : ����ū �ּ� ��ȯ
 * ----------------------------------------------------------------------------------
 */
int found_bigaddr(int csec) {
	int temp = 0;
	int i = -0;
	while (i < line_num) {
		if (token_table[i]->csect == csec) {
			if (token_table[i]->locctr > temp) {
				temp = token_table[i]->locctr;
			}
		}
		i++;
	}
	return temp;
}
