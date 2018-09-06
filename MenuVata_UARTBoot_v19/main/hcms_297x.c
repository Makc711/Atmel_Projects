/* �������� ������� ��� ������ � ������� HCMS-29xx
 * ����������� �������������� ������� ������� ����������, �� ����� �� ������
 * ������� �� ����������.
 */

#include "hcms_297x.h"

// ������ ��� ������������� ���� � ������ �������� GCC, ��������������� __flash
#if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 3))
#define p_get(x)	*(x)
#else
#define __flash		PROGMEM
#define p_get(x)	pgm_read_byte(x)
#endif

// ����� ���� �������� ������ ����� ����������� �������, ����� ����� ������
#include "hcms_297x_font5x7.h"

char hcms_screen[SCR_SIZE]; // �������� ���������� ����� (SCR_SIZE - ���������� �������� �� �������)

#if defined(USE_CURSOR)
static uint8_t cursor_pos = NO_CURSOR;
#endif

// ����������� ��������:
static ctrl_reg0_t def_ctrl_r0;
static ctrl_reg1_t def_ctrl_r1;


#define hcms_ctrl_mode()	PORT(PORT_RS) |= _BV(PIN_RS)  // ������ ��������� ������ ������ � �������� ���������� | 1 �� RS
#define hcms_data_mode()	PORT(PORT_RS) &= ~_BV(PIN_RS) // ������ ��������� ������ ������ ������	              | 0 �� RS
#define hcms_enable()		PORT(PORT_CE) &= ~_BV(PIN_CE) // ������ ��������� ����� �������                       | 0 �� CE
#define hcms_disable()		PORT(PORT_CE) |= _BV(PIN_CE)  // ������ ����������� ����� �������                     | 1 �� CE
#if !defined(USE_SPI)    // ���� �� ������������ ���������� SPI
#define hcms_clk_hi()		PORT(PORT_CLK) |= _BV(PIN_CLK)  // ������ ��������� �������� ������ � ����� CLK  | 1 �� CLK
#define hcms_clk_lo()		PORT(PORT_CLK) &= ~_BV(PIN_CLK) // ������ ��������� ������� ������ � ����� CLK   | 0 �� CLK
#define hcms_di_hi()		PORT(PORT_DI) |= _BV(PIN_DI)  // ������ ��������� �������� ������ � ����� ������ | 1 �� DI
#define hcms_di_lo()		PORT(PORT_DI) &= ~_BV(PIN_DI) // ������ ��������� ������� ������ � ����� ������  | 0 �� DI
#else
#define hcms_clk_hi() // ������ ��������� �������� ������ � ����� CLK
#define hcms_clk_lo() // ������ ��������� ������� ������ � ����� CLK
#define hcms_di_hi() // ������ ��������� �������� ������ � ����� ������
#define hcms_di_lo() // ������ ��������� ������� ������ � ����� ������
#endif

/** ����� �������
 * ����������� ������������ ��� - ��� �������������
 */
static void hcms_reset(void){
	PORT(PORT_RST) &= ~_BV(PIN_RST); // ��������� ������� ������ �� RST
	PORT(PORT_RST) |= _BV(PIN_RST);  // ��������� �������� ������ �� RST
}

/** ����� ����� ������� ������� � �������
 * ��������� ���������������� ����� ����� � ����������� �� �������� #USE_SPI
 * ���� ����������, ���� ���������
 * ���������� ����� �������� � 5 ��� ������� ������������
 * @param c
 */
#if !defined(USE_SPI) // ���� �� ������������ ���������� SPI
static void hcms_put_byte(uint8_t c){
	for(uint8_t mask = 0x80; mask; mask >>= 1){
		hcms_clk_lo(); // ��������� ������� ������ � ����� CLK | 0 �� CLK
		if(c & mask)	hcms_di_hi(); // ��������� �������� ������ � ����� ������ | 1 �� DI
		else			hcms_di_lo(); // ��������� ������� ������ � ����� ������ | 0 �� DI
		hcms_clk_hi(); // ��������� �������� ������ � ����� CLK | 1 �� CLK
	}
}
#else
static void hcms_put_byte(uint8_t c){
	SPDR = c;
	while(!(SPSR & _BV(SPIF)));
}
#endif

/** ���������� �������� ����������
 * @param data - ��������� �������� ���������� � ���� ������ �����
 */
static void hcms_ctrl_register(uint8_t data){
	hcms_ctrl_mode(); // ��������� ������ ������ � �������� ���������� | 1 �� RS
	hcms_enable(); // ��������� ����� ������� | 0 �� CE
	hcms_put_byte(data);
	hcms_clk_lo(); // ��������� ������� ������ � ����� CLK | 0 �� CLK
	hcms_disable(); // ����������� ����� ������� | 1 �� CE
}

// ����� "�����" ������
void hcms_raw_pixels(uint8_t *buf, buf_size_t sz){
	hcms_data_mode();
	hcms_enable();
	for(buf_size_t i=0; i < sz; i++){
		hcms_put_byte(buf[i]);
	}
	hcms_clk_lo();
	hcms_disable();
}

static void rpad_strcpy(char *d, char *s){ // �������� ������ *s � ������ *d
	uint8_t l = strlen(s); // ���������� ���������� �������� � ������ � ���������� "l"
	for(uint8_t i=0; i<SCR_SIZE; i++)
		if(i >= l)
			d[i] = ' '; // ���� ������� � ������ �����������, �������� ������� ������ ����������
		else
			d[i] = s[i]; // ����� ���������� ��� ������� � �����
}

// ����� ������
void hcms_puts(char *s){
	rpad_strcpy(hcms_screen, s); // ���������� � �������� ���������� ����� ���� �������� ������
	hcms_update_scr();
}

// ����� ������ �� flash
void hcms_puts_P(const char *s){
	char tmp[SCR_SIZE];
	strncpy_P(tmp, s, SCR_SIZE);
	hcms_puts(tmp);
}

/** ���������� �������� ������ ������� ������� � ������
 * ������������ ��������� � ��������� Win-1251
 * @param c - ������
 * @return �������� ������ ������� � ������� ������
 */
static const __flash uint8_t* get_offset_char(char c){
	const __flash uint8_t *ptr;
	char fc;

	fc = FONT_FIRST_CHAR;
	ptr = font5x7;

	switch(c){
	case FONT_FIRST_CHAR ... FONT_LAST_CHAR:
		break;
#if defined(SUPPORT_CYR)
	case '�' ... '�':
		fc = '�';
		ptr = font_cyr;
		break;
	case '�':
		fc = c;
		ptr = font_YO;
		break;
	case '�':
		fc = c;
		ptr = font_yo;
		break;
#endif
	case '\0':
		c = ' ';
		break;
	default:
		c = FONT_UNKNOWN_CHAR;
	}

	return ptr + (c - fc)*FONT_COLS;
}

#if defined(USE_CURSOR)
static uint8_t inverce;
#endif

static void hcms_put_char(char c){ // ��������� ������ �� �������
	const __flash uint8_t * font; // �������� ������ ������� � ������
	font  = get_offset_char(c); // ��������� ��������
	// ����� "���������" �������
	for(uint8_t i = 0; i < FONT_COLS; i++){ // ��� ������� "��������" �������
		// ��� �������� ����� �� flash ������������ ������ ������������� #p_get
		hcms_put_byte(p_get(font++)
#if defined(USE_CURSOR)
			// ���� ��������� ������, �� � ������ ������� �������� �������
			^ (inverce ? 0xFF : 0)
#endif
		);
	}
}

// ������� �������
void hcms_clrscr(void){
#if defined(USE_CURSOR)
	cursor_pos = NO_CURSOR;
#endif
	hcms_puts_P(PSTR(""));
}

// ���������� ������� ���������� �������� ������� (������)
void hcms_update_scr(void){
	hcms_data_mode(); // ��������� ������ ������ ������	| 0 �� RS
	hcms_enable(); // ��������� ����� ������� | 0 �� CE

	for(uint8_t pos = 0; pos < SCR_SIZE; pos++){ // ��� ������� ������� �� �������
#if defined(USE_CURSOR)
		// ���� ��������� ������, �� � ������ ������� �������� �������
		inverce = (pos == cursor_pos);
#endif
		hcms_put_char(hcms_screen[pos]); // ��������� ������ �� �������
	}
	hcms_clk_lo(); // ��������� ������� ������ � ����� CLK | 0 �� CLK
	hcms_disable(); // ����������� ����� ������� | 1 �� CE
}

#if defined(USE_CURSOR)
// ��������� ������� �������
void hcms_cursor_pos(uint8_t pos){
	cursor_pos = pos;
	hcms_update_scr();
}
#endif

// ���������� ��������
void hcms_bright(pwm_brightness_t br){
	def_ctrl_r0.brightness = br;
	hcms_ctrl_register(def_ctrl_r0.byte);
}

// ���������-���������� �������
void hcms_on(uint8_t on){
	def_ctrl_r0.sleep_mode = !on;
	hcms_ctrl_register(def_ctrl_r0.byte);
}

// ���������� ������� �����
void hcms_peak_current(peak_current_t pc){
	def_ctrl_r0.peak_current = pc;
	hcms_ctrl_register(def_ctrl_r0.byte);
}

// ������������� ������
INIT(7){
#if defined(PORT_HCMS)
	DDR(PORT_HCMS) |= _BV(PIN_CE) | _BV(PIN_RS) | _BV(PIN_RST); // ����� ����� - ������
	PORT(PORT_HCMS) |= _BV(PIN_CE) | _BV(PIN_RS) | _BV(PIN_RST); // ��������� �� � 1
#else
	DDR(PORT_CE) |= _BV(PIN_CE);
	PORT(PORT_CE) |= _BV(PIN_CE);
	DDR(PORT_RS) |= _BV(PIN_RS);
	PORT(PORT_RS) |= _BV(PIN_RS);
	DDR(PORT_RST) |= _BV(PIN_RST);
#endif
	hcms_reset();

#if defined(USE_SPI)
	// ������������� ����������� SPI
	SPCR = _BV(SPE) | _BV(MSTR) | SPI_CLK_DIV_4;
	SPSR = _BV(SPI2X);
#if defined(__AVR_ATmega32__) || defined(__AVR_ATmega32A__) || defined(__AVR_ATmega16__)
	DDRB |= _BV(PB7) | _BV(PB5) | _BV(PB4); // SS, MOSI, SCK
	DDRB &= ~ _BV(PB6); // MISO
#elif defined(__AVR_ATmega8__)
	DDRB |= _BV(PB2) | _BV(PB5) | _BV(PB3); // SS, MOSI, SCK
	DDRB &= ~_BV(PB4); // MISO
#else
#	error CONFIGURE MANUALLY SS PIN AS OUTPUT, and so MOSI and CLK.
#endif
#else
#if defined(PORT_HCMS)
	DDR(PORT_HCMS) |= _BV(PIN_CLK) | _BV(PIN_DI);
#else
	DDR(PORT_CLK) |= _BV(PIN_CLK);
	DDR(PORT_DI) |= _BV(PIN_DI);
#endif
	PORT(PORT_CLK) &= ~_BV(PIN_CLK);
#endif

	// ������ ��������� �� ��������� ��������� ����������
	def_ctrl_r0.brightness = DEFAULT_BRIGHTNESS;
	def_ctrl_r0.peak_current = DEFAULT_PEAK_CURRENT;
	def_ctrl_r0.sleep_mode = 1;
	def_ctrl_r0.reg = 0;

	def_ctrl_r1.data_out_ctrl = 1;
	def_ctrl_r1.edo_prescaler = 0;
	def_ctrl_r1.reserved = 0;
	def_ctrl_r1.reg = 1;

	hcms_clrscr();
	// � ������� ���������� 1 ���� �������� ��� ������������
	// �������� ��������� ����������, ����� � ����������� ��������� ���
	// �������� ������������ � ����� ����� ������ �����. �� ��� �������������
	// �������� ��������� ������ ������������ ����� � 1-� ������� ������� ���,
	// ������� ����� ��������� �����.
	for(uint8_t i=0; i < CHIP_CNT+1; i++)
		hcms_ctrl_register(def_ctrl_r1.byte);
	hcms_ctrl_register(def_ctrl_r0.byte);
}

/*****************************************************************************
 * ����� ������� "���������������" �������, ������������ �������� �������
 * �����������, �� ���� ��� ��������� ������� �������� ������������.
 *****************************************************************************/

/** ����� ������ ������� ������ ������ ����
 * ������� �������� �� ������� ���� ������ ����, �������� ������ ����������
 * �������� ������� �������
 * @param c0 - ���������� ������ (������)
 * @param c1 - ���������� ������ (�����)
 * @param shift - ����� ������ � ������� (1..8)
 */
static void shiftdn_two_char(uint8_t c0, uint8_t c1, uint8_t shift){
	uint8_t b0, b1;
	for(uint8_t i=0; i < FONT_COLS; i++){ // FONT_COLS - ���������� �������� � ���������� (5)
		b0 = p_get(get_offset_char(c0) + i); // ��� ������� ������� �������
		b1 = p_get(get_offset_char(c1) + i); // ��� ������� ������ �������
		for(uint8_t sh = shift; sh; sh--){ // ��� ������ ������ ������ �������
			b0 = (b0 << 1) | (b1 & 0x80 ? 1 : 0); // ���� b1 & 0x80 - ������, �� |1 ����� |0
			b1 <<= 1; // ��������� � ���������� ���� ���� �������
		}
		hcms_put_byte(b0);
	}
}

/** ����� ������ ������� ������ ����� �����
 * ������� �������� �� ������� ���� ������ �����, �������� ����� ����������
 * �������� ������� �������
 * @param c0 - ���������� ������ (������)
 * @param c1 - ���������� ������ (�����)
 * @param shift - ����� ������ � ������� (1..8)
 */
static void shiftup_two_char(uint8_t c0, uint8_t c1, uint8_t shift){
	uint8_t b0, b1;
	for(uint8_t i=0; i < FONT_COLS; i++){
		b0 = p_get(get_offset_char(c0) + i);
		b1 = p_get(get_offset_char(c1) + i);
		for(uint8_t sh = shift; sh; sh--){
			b0 = (b0 >> 1) | (b1 & 1 ? 0x80 : 0);
			b1 >>= 1;
		}
		hcms_put_byte(b0);
	}
}

/** �������� ������� ������ �� "��� ����"
 * �������������� �������
 * @param buf - ����������� ������ ������
 * @param sz - ���������� ����������� ������
 * @return 0, ���� ���� ���� �� ���� ��������� ����, 1 - ���� ��� ����� �
 * ������� ����� ����
 */
static uint8_t is_clear(uint8_t *buf, uint8_t sz){
	uint8_t result = 0;
	for(;sz;sz--)
		result |= *buf++;
	return result == 0;
}

static char ns[SCR_SIZE]; // ��������������� ����� ��� ���������� ��������

/** ������ ������� �� �������� ������� �� ����� �� ���������������� ������ (������ ��� ������������ ��������)
 * ��������� ������ ������ �������� ��� ���������
 * ����� (��������) ��� ������ (���������)
 * @param up - �����������: 1 - �����, -1 - ����
 * @param pos - �������������� ������
 * @param shift - ����� ������ � ������� (1..8)
 */
static void hcms_rollover_chars(int8_t up, uint8_t pos, uint8_t shift){
	if(hcms_screen[pos] != ns[pos]){ // ���� ������ �������� ������� �� ��������� � �������� ����� ������
		if(up < 0)	shiftdn_two_char(hcms_screen[pos], ns[pos], shift);
		else		shiftup_two_char(hcms_screen[pos], ns[pos], shift);
	} else {
		hcms_put_char(ns[pos]);
	}
}

/** ������ ������� �� �������� ������� �� ����� �� ���������������� ������ (��� ���� �������� ������)
 * ��������� ������ ������ �������� ��� ���������
 * ����� (��������) ��� ������ (���������)
 * @param up - �����������: 1 - �����, -1 - ����
 * @param pos - �������������� ������
 * @param shift - ����� ������ � ������� (1..8)
 */
static void hcms_rollover_chars_all(int8_t up, uint8_t pos, uint8_t shift){
	if(up < 0)	shiftdn_two_char(hcms_screen[pos], ns[pos], shift);
	else		shiftup_two_char(hcms_screen[pos], ns[pos], shift);
}

// ����� ������ � �������� ����������/��������� (������ ��� ������������ ��������)
// ����������� ������� �� ��������, �� ����������� - ���������� ������
void hcms_rollower_puts(char *s, int8_t up){
	rpad_strcpy(ns, s); // �������� � ������������� ����� ������ (16 ��������! �������� - �������)
	for(uint8_t sh=1; sh <=8; sh++){ // ���� �� ���������� ����� � �������
		hcms_data_mode(); // ��������� ������ ������ ������	| 0 �� RS
		hcms_enable(); // ��������� ����� ������� | 0 �� CE
		// ��������� ������� ����������
		for(uint8_t pos=0; pos < SCR_SIZE; pos++){
			hcms_rollover_chars(up, pos, sh); // ��������� ������ �������� ������� �� ������� ����� �� ������
		}
		hcms_clk_lo(); // ��������� ������� ������ � ����� CLK | 0 �� CLK
		hcms_disable(); // ����������� ����� ������� | 1 �� CE
		_delay_ms(ANIMATE_DELAY_MS); // �������� ��� �����������
	}
	memcpy(hcms_screen, ns, SCR_SIZE); // ������� ����� ������ � �������� �������
}

// ����� ������ � �������� ����������/��������� (��� ���� �������� ������)
// ��� ������� ������ ���������� ������
void hcms_rollower_puts_all(char *s, int8_t up){
	rpad_strcpy(ns, s); // �������� � ������������� ����� ������ (16 ��������! �������� - �������)
	for(uint8_t sh=1; sh <=8; sh++){ // ���� �� ���������� ����� � �������
		hcms_data_mode(); // ��������� ������ ������ ������	| 0 �� RS
		hcms_enable(); // ��������� ����� ������� | 0 �� CE
		// ��������� ������� ����������
		for(uint8_t pos=0; pos < SCR_SIZE; pos++){
			hcms_rollover_chars_all(up, pos, sh); // ��������� ������ �������� ������� �� ������� ����� �� ������
		}
		hcms_clk_lo(); // ��������� ������� ������ � ����� CLK | 0 �� CLK
		hcms_disable(); // ����������� ����� ������� | 1 �� CE
		_delay_ms(ANIMATE_DELAY_MS); // �������� ��� �����������
	}
	memcpy(hcms_screen, ns, SCR_SIZE); // ������� ����� ������ � �������� �������
}

// ����� ������ � �������� ������ �����
// ��� ������� ������ ���������� ������
void hcms_shiftleft_puts(char *s){
	rpad_strcpy(ns, s); // �������� � ������������� ����� ������ (16 ��������! �������� - �������)
	for(int8_t pos=-5; pos <=SCR_SIZE; pos++){ // ��� ���� �������� ������
		for(uint8_t i=SCR_SIZE-1; i>0; i--){
			hcms_screen[i] = hcms_screen[i-1]; // �������� ������ ��� ������� ����� �������
		}
		if (pos<1)
			hcms_screen[0] = ' '; // ���������� ������ � ������ ������ ������ ��� ���������� �����
		else
			hcms_screen[0] = ns[SCR_SIZE-pos]; // ���������� ����� ������ � ������ ������ ������
		hcms_puts(hcms_screen); // ����� ������ �� �������
		_delay_ms(ANIMATE_DELAY_MS); // �������� ��� �����������
	}
	memcpy(hcms_screen, ns, SCR_SIZE); // ������� ����� ������ � �������� �������
}

// ����� ������ � �������� ������ ������
// ��� ������� ������ ���������� ������
void hcms_shiftright_puts(char *s){
	rpad_strcpy(ns, s); // �������� � ������������� ����� ������ (16 ��������! �������� - �������)
	for(int8_t pos=-6; pos <SCR_SIZE; pos++){ // ��� ���� �������� ������
		for(uint8_t i=0; i <SCR_SIZE-1; i++){
			hcms_screen[i] = hcms_screen[i+1]; // �������� ����� ��� ������� ����� ����������
		}
		if (pos<0)
		hcms_screen[SCR_SIZE-1] = ' '; // ���������� ������ � ��������� ������ ������ ��� ���������� �����
		else
		hcms_screen[SCR_SIZE-1] = ns[pos]; // ���������� ����� ������ � ��������� ������ ������
		hcms_puts(hcms_screen); // ����� ������ �� �������
		_delay_ms(ANIMATE_DELAY_MS); // �������� ��� �����������
	}
	memcpy(hcms_screen, ns, SCR_SIZE); // ������� ����� ������ � �������� �������
}

// ����� ������ � �������� ����������������� ����������/��������� (������ ��� ������������ ��������)
// ����������� ������� �� ��������, �� ����������� - ���������� ������ (������ ��������� ������ ����������� ����� �����������)
void hcms_smooth_rollower_puts(char *s, int8_t up){
	rpad_strcpy(ns, s); // �������� � ������������� ����� ������ (16 ��������! �������� - �������)

	static uint8_t sh[SCR_SIZE];	// ������ ����� ������
	memset(sh, 0, SCR_SIZE);		// ���������� �� �������,
	sh[0] = 1;						// ����� ������ ������� ��������

	// ���� ���������� ������, ���� � ������� ������� ���� ���� �� ����
	// �� ����������� ���
	while(!is_clear(sh, SCR_SIZE)){
		hcms_data_mode();
		hcms_enable();

		// ������ ������� �� ������� �������� ��� ������ ������ ����� ��������
		for(uint8_t pos=0; pos < SCR_SIZE; pos++){
			// ����� � ������ ������� ����� ���� �������� - ������ �������
			hcms_rollover_chars(up, pos, sh[pos]);
		}
		// ��������� ������ �������, ����������� ������� ���������� ������
		// ������� ���������� ������� �� 1 �������
		memmove(sh+1, sh, SCR_SIZE-1);
		if(sh[0]){ // ���� ������ ������� �� ����� ����
			if(++(sh[0]) > 8){ // ����������� ��� �� 1, �� �� ������, ��� �� 8
				// ���� ���������, ��� ��������� ����� �� 9 - ��������,
				// �.�. �������� ������ �� ����
				sh[0] = 0;
			}
		}

		// ��������� �������� �������
		for(uint8_t i=0; i < SCR_SIZE; i++){
			// �������� � ��� ��� ������� ����� ������, ��� ������� ����� ��� ��������
			if(sh[i] == 0) hcms_screen[i] = ns[i];
			else break;
		}
		hcms_clk_lo();
		hcms_disable();
		_delay_ms(ANIMATE_DELAY_MS); // �������� ��� �����������
	}
}

// ����� ������ � �������� ����������������� ����������/��������� (��� ���� �������� ������)
// ����������� ������� �� ��������, �� ����������� - ���������� ������ (������ ��������� ������ ����������� ����� �����������)
void hcms_smooth_rollower_puts_all(char *s, int8_t up){
	rpad_strcpy(ns, s); // �������� � ������������� ����� ������

	static uint8_t sh[SCR_SIZE];	// ������ ����� ������
	memset(sh, 0, SCR_SIZE);		// ���������� �� �������,
	sh[0] = 1;						// ����� ������ ������� ��������

	// ���� ���������� ������, ���� � ������� ������� ���� ���� �� ����
	// �� ����������� ���
	while(!is_clear(sh, SCR_SIZE)){
		hcms_data_mode();
		hcms_enable();

		// ������ ������� �� ������� �������� ��� ������ ������ ����� ��������
		for(uint8_t pos=0; pos < SCR_SIZE; pos++){
			// ����� � ������ ������� ����� ���� �������� - ������ �������
			hcms_rollover_chars_all(up, pos, sh[pos]);
		}
		// ��������� ������ �������, ����������� ������� ���������� ������
		// ������� ���������� ������� �� 1 �������
		memmove(sh+1, sh, SCR_SIZE-1);
		if(sh[0]){ // ���� ������ ������� �� ����� ����
			if(++(sh[0]) > 8){ // ����������� ��� �� 1, �� �� ������, ��� �� 8
				// ���� ���������, ��� ��������� ����� �� 9 - ��������,
				// �.�. �������� ������ �� ����
				sh[0] = 0;
			}
		}

		// ��������� �������� �������
		for(uint8_t i=0; i < SCR_SIZE; i++){
			// �������� � ��� ��� ������� ����� ������, ��� ������� ����� ��� ��������
			if(sh[i] == 0) hcms_screen[i] = ns[i];
			else break;
		}
		hcms_clk_lo();
		hcms_disable();
		_delay_ms(ANIMATE_DELAY_MS); // �������� ��� �����������
	}
}