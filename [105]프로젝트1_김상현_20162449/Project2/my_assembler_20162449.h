#pragma once
/*
 * my_assembler 함수를 위한 변수 선언 및 매크로를 담고 있는 헤더 파일이다.
 *
 */
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3

 //int exturn_i = 0;//new

 /*
  * instruction 목록 파일로 부터 정보를 받아와서 생성하는 구조체 변수이다.
  * 라인 별로 하나의 instruction을 저장한다.
  */
	struct inst_unit
  {
	  char str[10];
	  unsigned char op;
	  int format;
	  int ops;
  };
  typedef struct inst_unit inst;
  inst* inst_table[MAX_INST];
  int inst_index;

  /*
   * 어셈블리 할 소스코드를 입력받는 테이블이다. 라인 단위로 관리할 수 있다.
   */
  char* input_data[MAX_LINES];
  static int line_num;

  /*
   * 어셈블리 할 소스코드를 토큰단위로 관리하기 위한 구조체 변수이다.
   * operator는 renaming을 허용한다.
   */
  struct token_unit
  {
	  int csect;
	  int locctr;
	  char* label;
	  char* operator;
	  char operand[MAX_OPERAND][20];
	  char comment[100];
	  // 다음과제에 사용될 변수
	  char nixbpe;
  };

  typedef struct token_unit token;
  token* token_table[MAX_LINES];
  static int token_line;

  /*
   * 심볼을 관리하는 구조체이다.
   * 심볼 테이블은 심볼 이름, 심볼의 위치로 구성된다.
   * 추후 과제에 사용 예정
   */
  struct symbol_unit
  {
	  char* symbol;
	  int addr;
	  int csec;
  };
  typedef struct symbol_unit symbol;
  symbol sym_table[MAX_LINES];

  struct literal_unit
  {
	  int cesc_num;
	  int addr;
	  char* literal;
  };
  typedef struct literal_unit literal;
  literal literal_table[MAX_LINES]; //literal 구조체

  static int locctr;
  //--------------

  static char* input_file;
  static char* output_file;
  int init_my_assembler(void);
  int init_inst_file(char* inst_file);
  int init_input_file(char* input_file);
  int token_parsing(char* str);


  int search_opcode(char* str);
  static int assem_pass1(void);
  void input_locctr_token_table(void);

  void make_symtab_output(char* file_name);
  static int assem_pass2(void);

  void make_opcode_output(char* file_name);
  void make_symtab_output(char* file_name);
  void make_literaltab_output(char* file_name);
  static int assem_pass2(void);
  void make_objectcode_output(char* file_name);

  int StringToHexNum(char* input); //new
  void check_Enter(char* input);//new
  int Search_LTORG(void);//new
  int Search_Label(char* label);//new

  static int literal_line;//new
  static int symbal_line;//new

  static int search_ltorg_index;//new

  int addr_found(char* str, int csec_num);//new


  struct opcode_unit//new
  {
	  int csec_num;
	  int addr;
	  int opcode;
	  int format;
  };

  typedef struct opcode_unit opcode;
  opcode opcode_table[MAX_LINES]; //object code를 모아두기 위한 table


  static int opcode_table_line_num;
  static int csec_num;
  static int strloc;