/* transform an oid mode in a beautiful string */
char *parse_oid_mode(unsigned int mode) {
	static char buff[sizeof(char)*7];
	buff[6] = '\0';
	buff[5] = '0' + (mode & 0x7);
	buff[4] = '0' + ((mode >> 3) & 0x7);
	buff[3] = '0' + ((mode >> 6) & 0x7);
	buff[2] = '0';
	buff[1] = '0' + ((mode >> 12) & 0x7);
	buff[0] = '0' + (mode >> 15);
	return buff;
}