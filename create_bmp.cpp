#include <cstdio>
#include <cstring>
#include <cmath>
#include <vector>

#pragma warning(push)
#pragma warning(disable: 4996)

bool create_bmp(int width, int height, const char * file_path, const unsigned char * data_bgr)
{
	//RGBデータの編集
	std::vector<unsigned char> sliced_bgr;

	for (int i = 0; i<abs(height); i++){
		for (int j = 0; j<width * 3; j++) sliced_bgr.push_back(data_bgr[i*width * 3 + j]);
		for (int j = 0; j<width % 4; j++) sliced_bgr.push_back('\0');
	}

	//ヘッダデータの作成
	char header_buf[54];

	for (int i = 0; i<54; i++) header_buf[i] = 0;

	header_buf[0] = 'B';
	header_buf[1] = 'M';
	int real_width = width * 3 + width % 4;
	long data_size = abs(height) * real_width;
	int file_size = data_size + 54;
	memcpy(header_buf + 2, &file_size, sizeof(file_size));
	header_buf[10] = 54;
	header_buf[14] = 40;
	memcpy(header_buf + 18, &width, sizeof(width));
	memcpy(header_buf + 22, &height, sizeof(height));
	int planes = 1;
	memcpy(header_buf + 26, &planes, sizeof(planes));
	int color = 24;
	memcpy(header_buf + 28, &color, sizeof(color));
	long compress = 0;
	memcpy(header_buf + 30, &compress, sizeof(compress));
	memcpy(header_buf + 34, &data_size, sizeof(data_size));

	//ファイル出力
	FILE *fp = fopen(file_path, "wb");
	if (fp == NULL) return 0;

	fwrite(header_buf, sizeof(char), 54, fp);
	fwrite(&(sliced_bgr[0]), sizeof(char), data_size, fp);

	fclose(fp);

	return 1;
}

#pragma warning(pop)