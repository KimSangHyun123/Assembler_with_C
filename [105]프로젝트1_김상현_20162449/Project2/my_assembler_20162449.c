/*
 * 화일명 : my_assembler_20162449.c
 * 설  명 : 이 프로그램은 SIC/XE 머신을 위한 간단한 Assembler 프로그램의 메인루틴으로,
 * 입력된 파일의 코드 중, 명령어에 해당하는 OPCODE를 찾아 출력한다.
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h >
#include <stdlib.h >
#include <string.h >
#include <fcntl.h >
#include "my_assembler_20162449.h"
 /* ----------------------------------------------------------------------------------
  * 설명 : 사용자로 부터 어셈블리 파일을 받아서 명령어의 OPCODE를 찾아 출력한다.
  * 매계 : 실행 파일, 어셈블리 파일
  * 반환 : 성공 = 0, 실패 = < 0
  * 주의 : 현재 어셈블리 프로그램의 리스트 파일을 생성하는 루틴은 만들지 않았다.
  *		   또한 중간파일을 생성하지 않는다.
  * ----------------------------------------------------------------------------------
  */
int main(int args, char* arg[])
{
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: 프로그램 초기화에 실패 했습니다.\n");
		return -1;
	}
	if (assem_pass1() < 0)
	{
		printf("assem_pass1: 패스1 과정에서 실패하였습니다.  \n");
		return -1;
	}
	make_symtab_output("symtab_20162449");
	make_literaltab_output("literaltab_20162449");
	if (assem_pass2() < 0)
	{
		printf(" assem_pass2: 패스2 과정에서 실패하였습니다.  \n");
		return -1;
	}
	make_objectcode_output("output_20162449");
	return 0;
}
/* ----------------------------------------------------------------------------------
 * 설명 : 프로그램 초기화를 위한 자료구조 생성 및 파일을 읽는 함수이다.
 * 매계 : 없음
 * 반환 : 정상종료 = 0 , 에러 발생 = -1
 * 주의 : 각각의 명령어 테이블을 내부에 선언하지 않고 관리를 용이하게 하기
 *		   위해서 파일 단위로 관리하여 프로그램 초기화를 통해 정보를 읽어 올 수 있도록
 *		   구현하였다.
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
 * 설명 : 머신을 위한 기계 코드목록 파일을 읽어 기계어 목록 테이블(inst_table)을
 *        생성하는 함수이다.
 * 매계 : 기계어 목록 파일
 * 반환 : 정상종료 = 0 , 에러 < 0
 * 주의 : 기계어 목록파일 형식은 자유롭게 구현한다. 예시는 다음과 같다.
 *
 *	===============================================================================
 *		   | 이름 | 형식 | 기계어 코드 | 오퍼랜드의 갯수 | NULL|
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
 * 설명 : 문자열이지만 16진수의 의미를 담고 있기 때문에 이를 저장하기 위해서는 문자열을 정수형으로 바꿔줘야한다.
		  ex ) 1C -> 1*16 + 12
 * 매계 : 문자열
 * 반환 : 없음
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
 * 설명 : 어셈블리 할 소스코드를 읽어 소스코드 테이블(input_data)를 생성하는 함수이다.
 * 매계 : 어셈블리할 소스파일명
 * 반환 : 정상종료 = 0 , 에러 < 0
 * 주의 : 라인단위로 저장한다.
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
* 설명 : 어셈블리 코드를 위한 패스1과정을 수행하는 함수이다.
*		   패스1에서는..
*		   1. 프로그램 소스를 스캔하여 해당하는 토큰단위로 분리하여 프로그램 라인별 토큰
*		   테이블을 생성한다.
*
* 매계 : 없음
* 반환 : 정상 종료 = 0 , 에러 = < 0
* 주의 : 현재 초기 버전에서는 에러에 대한 검사를 하지 않고 넘어간 상태이다.
*	  따라서 에러에 대한 검사 루틴을 추가해야 한다.
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
	/* input_data의 문자열을 한줄씩 입력 받아서
	 * token_parsing()을 호출하여 token_unit에 저장
	 */
}
/* ----------------------------------------------------------------------------------
 * 설명 : 소스 코드를 읽어와 토큰단위로 분석하고 토큰 테이블을 작성하는 함수이다.
 *        패스 1로 부터 호출된다.
 * 매계 : 파싱을 원하는 문자열
 * 반환 : 정상종료 = 0 , 에러 < 0
 * 주의 : my_assembler 프로그램에서는 라인단위로 토큰 및 오브젝트 관리를 하고 있다.
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
	// 주석 필터링
	if (str[0] == '.')
		return 0;
	// Tab을 구분자로 하여 각 필드 분리
	for (int i = 0; fields[i] != NULL; i++) {
		*fields[i] = str;
		if ((str = strpbrk(str, "\t\n")) == NULL)
			break;
		*str++ = '\0'; // 중간에 NULL 문자를 넣어 문자열 분리
	}
	for (int i = 0; operand_fields[i] != NULL; i++) {
		*operand_fields[i] = operand;
		if ((operand = strpbrk(operand, ",")) == NULL)
			break;
		*operand++ = '\0'; // 중간에 NULL 문자를 넣어 문자열 분리
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
* 설명 : 생성된 token_table에 locctr을 추가하는 함수
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
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
	while (i < line_num) {// 시작주소를 찾기위해 START 뒤의 operand 값을 저장한다.
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
				locctr += (3 * atoi(token_table[i]->operand[0]));  //word 는 1개당 3바이트!
			}
			else if (strcmp(token_table[i]->operator,"RESB") == 0) {
				locctr += atoi(token_table[i]->operand[0]);
			}
			else if (strcmp(token_table[i]->operator,"CSECT") == 0) { // 새로운 서브루틴 의미하기 떄문에 0오로 locctr 초기화
				token_table[i]->locctr = 0;
				locctr = 0;
			}
			else if (strcmp(token_table[i]->operator,"LTORG") == 0) { //LRORG 를 찾으면 
				locctr += (strlen(token_table[Search_LTORG(i)]->operand[0]) - 4); // Search_LTORG 함수로 이동하여 search_ltorg_index 부터 현재 index 까지에 '='이들어간 operand를 찾는다.
				search_ltorg_index = i;
			}
			else if (strcmp(token_table[i]->operator,"BYTE") == 0) {
				if (token_table[i]->operand[0][0] == 'X')
					locctr += (strlen(token_table[i]->operand[0]) - 3) / 2;
				//if(token_table[i]->operand[0][0] == '') // 이곳에 'X' 말고 다른 조건을 넣으면 된다.
			}
			else if (strchr(token_table[i]->operand[0], '-') != NULL && strcmp(token_table[i]->operator,"EQU") == 0) { // BUFFER -BUFEND 같은 경우에대한 예외 처리
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
 *설명 : (추가) LTORG 가 나왔을 때 그 위의 '='으로 시작하는 operand를 찾는다
 *매계 : void
 *반환 : 그 token_table의 index
 --------------------------------------------------------------------------------------------------------*/
int Search_LTORG(int index) {
	for (int i = search_ltorg_index; i < index; i++) {
		if (token_table[i]->operand[0][0] == '=') // =C'EOF' 라고 할때 operand[0][0]= '=' 이 되고 이때의 index를 반환.
			return i;
	}
}
/*------------------------------------------------------------------
* 설명 : 주어진 label이 존재하는 token_table의 inst_table의 op를 비교하여 존재하면 그 op를 반환,
		이때 +jsub 같은 경우가 존재할 수 있으므로 문자열 첫번째 문자를 없애고 다시 같은 과정 반복.
* 매계 : 문자열
* 반환 : 정상종료 = 기계어 테이블 인덱스, 에러 < 0
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
* 설명 : 주어진 input 문자열과 inst_table의 op를 비교하여 존재하면 그 op를 반환,
		이때 +jsub 같은 경우가 존재할 수 있으므로 문자열 첫번째 문자를 없애고 다시 같은 과정 반복.
* 매계 : 문자열
* 반환 : 정상종료 = 기계어 테이블 인덱스, 에러 < 0
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
	}//첫 문자 삭제
	tmp_input[j - 1] = '\0';
	for (i = 0; i < inst_index; i++) {
		if (strcmp(inst_table[i]->str, tmp_input) == 0)
			return i;
	}
	return -1;
}
/*------------------------------------------------------------------
* 설명 : fgets() 함수로 받은 각 라인들에 마지막에 개행문자가 들어가면서
		각 테이블로 파싱되는 과정에서 개행문자들이 섞여 들어가
		원하는 output.txt를 출력하지 못하길래 개행문자를 없애는 함수를 만들었습니다.
* 매계 : 문자열
* 반환 : 없음
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
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 SYMBOL별 주소값이 저장된 TABLE이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
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
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 LITERAL별 주소값이 저장된 TABLE이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
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
			for (int j = i; j < line_num; j++) {// literal 의 index 부터 ~ 끝까지 LTORG가 operator로 나오는지 검사한다.
				if (strcmp(token_table[j]->operator,"LTORG") == 0) {
					if (check_literal_table(token_table[i]->operand[0]) < 0) {//literal_table 중복검사
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
			if (flag == 0) { // '='이 포함된 operand 밑에 LTORG가 없다는 뜻!
				if (check_literal_table(token_table[i]->operand[0]) < 0) {//literal_table 중복검사
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
 * 설명 : literal table에 중복된 값이 들어가지 않도록 확인하는 함수이다.
 * 매계 : 문자열 (oeprand[0])
 * 반환 : 이미 존재하면 1 없다면 -1
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
* 설명 : 어셈블리 코드를 기계어 코드로 바꾸기 위한 패스2 과정을 수행하는 함수이다.
*		   패스 2에서는 프로그램을 기계어로 바꾸는 작업은 라인 단위로 수행된다.
*		   다음과 같은 작업이 수행되어 진다.
*			1. 실제로 해당 어셈블리 명령어를 기계어로 바꾸는 작업을 수행한다.
*			2. 어셈블리 명령어들을 opcode_table 저장한다.
* 매계 : 없음
* 반환 : 정상종료 = 0, 에러발생 = < 0
* 주의 :
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
		if (index >= 0) { // operator 가 inst_table 에 존재할때
			if (inst_table[index]->format == 3) {
				if (token_table[i]->operand[0][0] == '@') {//immidiate
					token_table[i]->nixbpe += 1;
					token_table[i]->nixbpe = token_table[i]->nixbpe << 5; // n:1 i:0 x:0 p:0 e:0
					token_table[i]->nixbpe += 2; // n:1 i:0 x:0 p:1 e:0 indirect 는 pc relative 사용
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
					temp_char[0] = '0'; // 문자열로 되어있는 " # '주소' " 에서 주소만 파싱하기 위해 '#'을 '0'으로 치환함
					//빈칸
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
				}// x 즉 루프문일때 
				if (token_table[i]->operator[0] == '+') { //4형식 일때
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
					temp = temp << 4;// op코드를 4번 왼쪽으로 쉬프트하여 nixbpe가 들어올 자리를 만든다.
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
				temp = temp << 4;// op코드를 4번 왼쪽으로 쉬프트하여 nixbpe가 들어올 자리를 만든다.
				temp += token_table[i]->nixbpe;
				temp = temp << 12;// opcode 와 nixbpe 가 더하여진 값이 뒤에 16진수 000을 넣기 위해 왼쪽으로 12번 시프트
				//printf("%.6x\n", temp);
				//printf("%s :%x,  %x \n", token_table[i]->operand[0],addr_found(token_table[i]->operand[0]), token_table[i + 1]->locctr);
				if (addr_found(token_table[i]->operand[0], csec_num) - token_table[i + 1]->locctr >= 0) {
					//printf("%x  %x\t", addr_found(token_table[i]->operand[0]), token_table[i + 1]->locctr);
					temp = temp + addr_found(token_table[i]->operand[0], csec_num) - token_table[i + 1]->locctr;
				}
				else {//target주소 - 현재 program counter <0 이라면 보수를 이용하여 푼다
					//printf("%s  %x  %x\t", token_table[i]->operand[0], addr_found(token_table[i]->operand[0]), token_table[i + 1]->locctr);
					temp += (4096 - token_table[i + 1]->locctr + addr_found(token_table[i]->operand[0], csec_num));
				}
				opcode_table[opcode_table_line_num].opcode = temp;
				opcode_table[opcode_table_line_num].format = 3;
				opcode_table[opcode_table_line_num].addr = token_table[i]->locctr;
				opcode_table[opcode_table_line_num].csec_num = token_table[i]->csect;
				opcode_table_line_num++;
			}
			else if (inst_table[index]->format == 2) {//2형식일때 
				temp = inst_table[index]->op << 8; //8번 시프트 했다는 것은 16진수 B4 라면 B400이 됨을 의미한다.
				if (strcmp(token_table[i]->operand[0], "X") == 0) {
					temp += (16 * 1); // 16을 곱하는 이유는 2번째 자리에 넣어야 하기 때문이다!
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
		else { //해당 operator가 inst_table에 존재하지 않을 때
			if (strcmp(token_table[i]->operator,"BYTE") == 0) {
				if (token_table[i]->operand[0][0] != '=') { // X'F1' 에 대한 예외 처리
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
				if (token_table[i]->operand[0][1] == 'X') { // =X'F1'에 대한 예외처리
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
				if (token_table[i]->operand[0][1] == 'C') { // =C' ' 일때
				//	printf("/%s\n", token_table[i]->operand[0]);
					strcpy(temp_char, token_table[i]->operand[0]);

					for (int k = 0; k < strlen(temp_char) - 4; k++) {// tamp_char이 "=C'EOF'" 일때 strlen (tamp_char)-4는 3, 즉 EOF 의 strlen이 나온다.
						temp_hex = temp_hex << (8); //16진수 상에서 한칸 옆으로 옮기기 위하여
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
 * 설명 : 주어진 csec_num와 문자열을 가지고 심볼테이블과 리터럴테이블을 뒤지면서 존재하는 지 찾는 함수이다.
 * 매계 : 문자열 서브루틴 번호
 * 반환 : 주소 반환
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
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 object code (프로젝트 1번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
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
 * 설명 : 같은 csec_num 중에서 제일 큰 주소를 찾기위한 함수이다.
 *		  object파일을 만들때 각각의 서브루틴의 크기를 저장한다.
 * 매계 : 서브루틴 번호
 * 반환 : 제일큰 주소 반환
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
