#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <unistd.h>
#include <dirent.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace	cv;
int cut_number2(Mat& img);

void identify_number_and_alphabet(Mat& img){

}

void identify_chinese_character(Mat& img){

}

//获取“天猫工商信息执照”文件夹下的图片文件
vector<string> get_file_name(){
    vector<string> files;

    char basePath[100];
    char tmp[100];
    memset(basePath, 0, sizeof(basePath));
    memset(tmp, 0, sizeof(tmp));

    getcwd(basePath, 99);
    sprintf(tmp, "%s/%s", basePath, "天猫工商信息执照");

    string path(tmp);
    DIR *dir;
    struct dirent *ptr;

    if ((dir = opendir(path.c_str())) == NULL){
    	cout<<"can not open dir"<<endl;
		exit(1);
    }

    while ((ptr = readdir(dir)) != NULL){
        if(strcmp(ptr->d_name,".") == 0 || strcmp(ptr->d_name,"..") == 0)
            continue;
        else if(ptr->d_type == 8 && (strstr(ptr->d_name, "png") || strstr(ptr->d_name, "jpeg")))
            files.push_back(ptr->d_name);
    }
    closedir(dir);

    return files;
}

//已知img图片是一行数字和字母混合，此函数将每一数字和字母切分成单字符
void cut_number(Mat& img){
	int row = img.rows;
	int col = img.cols;

	int sum_r[row] = {0};
	for (int i = 0; i < row; i++){
		for (int j = 0; j < col; j++){
			if (img.at<uchar>(i,j) == 255){
				sum_r[i]++;
			}
		}
	}

	int sum_c[col] = {0};
	for (int j = 0; j < col; j++){
		for (int i = 0; i < row; i++){
			if (img.at<uchar>(i,j) == 255)
				sum_c[j]++;
		}
	}


	int y1,y2;
	for (int i = 0; i < row-1; i++){
		if (sum_r[i] == 0 && sum_r[i+1] != 0){
			y1 = i+1;
		}

		if (sum_r[i] != 0 && sum_r[i+1] == 0){
			y2 = i;
		}
	}

	vector<Point > x_point;
	Point tmp;
	for (int i = 0; i < col-1; i++ ){

		if (sum_c[i] == 0 && sum_c[i+1] != 0){
			tmp.x = i+1;
		}

		if (sum_c[i] != 0 && sum_c[i+1] == 0){
			tmp.y = i;
			x_point.push_back(tmp);
		}
	}


	for (int i = 0; i < x_point.size(); i++){

		Mat image_cut = Mat(img, Rect(x_point[i].x, y1, x_point[i].y-x_point[i].x+1, y2-y1+1));
		
		if (x_point[i].y-x_point[i].x+1 > 30){
			int cut = cut_number2(image_cut);
			Mat image_cut_1 = Mat(image_cut, Rect(0, 0, cut, image_cut.rows));
			resize(image_cut_1, image_cut_1, Size(24,24));
			//识别数字和字母的代码
			identify_number_and_alphabet(image_cut_1);

			Mat image_cut_2 = Mat(image_cut, Rect(cut+1, 0, image_cut.cols-cut-1, image_cut.rows));
			resize(image_cut_2, image_cut_2, Size(24,24));
			//识别数字和字母的代码
			identify_number_and_alphabet(image_cut_1);

		}else{
			resize(image_cut, image_cut, Size(24,24));
			//识别数字和字母的代码
			identify_number_and_alphabet(image_cut);

		}
	}

}

//在cut_number函数中，存在两个字母粘连的情况，此函数将找到两个字母的中间位置，返回切分点的列号
int cut_number2(Mat& img){
	int row = img.rows;
	int col = img.cols;
	int positon_u[col] = {0};
	int positon_d[col] = {0};
	int diff[col] = {0};

	for (int j = 0; j < col; j++){
		for (int i = 0; i < row; i++){
			if (img.at<uchar>(i,j) == 255){
				positon_u[j] = i;
				break;
			}
		}
		for (int i = row-1; i >= 0; i++){
			if (img.at<uchar>(i,j) == 255){
				positon_d[j] = i;
				break;
			}
		}
	}

	int min_position = 100;
	int min_diff = 100;
	for (int i = 16; i < 20; i++){
		diff[i] = positon_d[i]-positon_u[i];
		if (diff[i] < min_diff){
			min_diff = diff[i];
			min_position = i;
		}
	}
	return min_position;

}

//已知img图片是一行汉字，此函数将每一汉子切分成单字符
void cut_word(Mat& img){
	int row = img.rows;
	int col = img.cols;

	int sum_r[row] = {0};
	for (int i = 0; i < row; i++){
		for (int j = 0; j < col; j++){
			if (img.at<uchar>(i,j) == 255){
				sum_r[i]++;
			}
		}
	}

	int sum_c[col] = {0};
	for (int j = 0; j < col; j++){
		for (int i = 0; i < row; i++){
			if (img.at<uchar>(i,j) == 255)
				sum_c[j]++;
		}
	}


	int y1,y2;
	for (int i = 0; i < row-1; i++){
		if (sum_r[i] == 0 && sum_r[i+1] != 0){
			y1 = i+1;
		}
		if (sum_r[i] != 0 && sum_r[i+1] == 0){
			y2 = i;
		}
	}

	int flag = 0;
	for (int i = 0; i < col; i++){
		flag++;
		if (sum_c[i] == 0){
			flag = 0;
		}

		if (flag == 26){
			sum_c[i] = 0;
			flag = 0;
		}
	}
	flag = 0;
	for (int i = 0; i < col; i++){
		flag++;
		if (sum_c[i] == 0 && sum_c[i+1] == 0 && flag == 1)
			flag = 0;
		if (sum_c[i] == 0 && flag < 26 && flag != 0){
			sum_c[i] = 1;
		}
		if (flag == 26){
			sum_c[i] = 0;
			flag = 0;
		}

	}


	vector<Point> x_point;
	Point tmp;
	for (int i = 0; i < col-1; i++ ){

		if (sum_c[i] == 0 && sum_c[i+1] != 0){
			tmp.x = i+1;
		}

		if (sum_c[i] != 0 && sum_c[i+1] == 0){
			tmp.y = i;
			x_point.push_back(tmp);
		}
	}

	for (int i = 0; i < x_point.size(); i++){
		
		Mat image_cut = Mat(img, Rect(x_point[i].x, y1, x_point[i].y-x_point[i].x+1, y2-y1+1));
		resize(image_cut, image_cut, Size(24,24));
		//识别汉字的代码
		identify_chinese_character(image_cut);

	}

}

//已知img图片是一行汉字，其中有括号，影响单个字符的分割，此函数可以先去除左右括号
void pre_cut_bracket(Mat& img){
	int row = img.rows;
	int col = img.cols;
	int bracket_l[24][7] = {
		0,0,0,0,1,1,1,
		0,0,0,1,1,1,0,
		0,0,1,1,1,1,0,
		0,0,1,1,1,0,0,
		0,1,1,1,0,0,0,
		0,1,1,1,0,0,0,
		1,1,1,0,0,0,0,
		1,1,1,0,0,0,0,
		1,1,1,0,0,0,0,
		1,1,1,0,0,0,0,
		1,1,0,0,0,0,0,
		1,1,0,0,0,0,0,
		1,1,0,0,0,0,0,
		1,1,0,0,0,0,0,
		1,1,1,0,0,0,0,
		1,1,1,0,0,0,0,
		1,1,1,0,0,0,0,
		1,1,1,0,0,0,0,
		0,1,1,1,0,0,0,
		0,1,1,1,0,0,0,
		0,0,1,1,1,0,0,
		0,0,1,1,1,0,0,
		0,0,0,1,1,1,0,
		0,0,0,0,1,1,1,
	};
	int bracket_r[24][7] = {
		1,1,1,0,0,0,0,
		0,1,1,1,0,0,0,
		0,1,1,1,1,0,0,
		0,0,1,1,1,0,0,
		0,0,0,1,1,1,0,
		0,0,0,1,1,1,0,
		0,0,0,0,1,1,1,
		0,0,0,0,1,1,1,
		0,0,0,0,1,1,1,
		0,0,0,0,1,1,1,
		0,0,0,0,0,1,1,
		0,0,0,0,0,1,1,
		0,0,0,0,0,1,1,
		0,0,0,0,0,1,1,
		0,0,0,0,1,1,1,
		0,0,0,0,1,1,1,
		0,0,0,0,1,1,1,
		0,0,0,0,1,1,1,
		0,0,0,1,1,1,0,
		0,0,0,1,1,1,0,
		0,0,1,1,1,0,0,
		0,0,1,1,1,0,0,
		0,1,1,1,0,0,0,
		1,1,1,0,0,0,0,
	};
	for (int i = 0; i < 24; i++){
		for (int j = 0; j < 7; j++){
			if (bracket_l[i][j] == 1)
				bracket_l[i][j] = 255;
			if (bracket_r[i][j] == 1)
				bracket_r[i][j] = 255;
		}
	}

	int flag = 0;
	for (int j = 0; j < col; j++){
		for (int i = 0; i < row-7; i++){
			if (flag == 0 &&
				img.at<uchar>(i,j)==0 &&
				img.at<uchar>(i,j+1)==0 &&
				img.at<uchar>(i,j+2)==0 &&
				img.at<uchar>(i,j+3)==0 &&
				img.at<uchar>(i,j+4)==255 &&
				img.at<uchar>(i,j+5)==255 &&
				img.at<uchar>(i,j+6)==255){

				int same = 0;
				for (int m = 0; m < 24; m++){
					for (int n = 0; n < 7; n++){
						if (img.at<uchar>(i+m,j+n) == bracket_l[m][n])
							same++;
					}
				}
				if (same*1.0/(24*7) > 0.95){
					flag = 1;
					for (int m = 0; m < 24; m++){
						uchar* data = img.ptr<uchar>(i+m);
						for (int n = 0; n < 7; n++){
							data[j+n] = 0;
						}
					}
				}
			}

			if (flag == 1 &&
				img.at<uchar>(i,j)==255 &&
				img.at<uchar>(i,j+1)==255 &&
				img.at<uchar>(i,j+2)==255 &&
				img.at<uchar>(i,j+3)==0 &&
				img.at<uchar>(i,j+4)==0 &&
				img.at<uchar>(i,j+5)==0 &&
				img.at<uchar>(i,j+6)==0){

				int same = 0;
				for (int m = 0; m < 24; m++){
					for (int n = 0; n < 7; n++){
						if (img.at<uchar>(i+m,j+n) == bracket_r[m][n])
							same++;
					}
				}
				if (same*1.0/(24*7) > 0.93){
					flag = 0;
					for (int m = 0; m < 24; m++){
						uchar* data = img.ptr<uchar>(i+m);
						for (int n = 0; n < 7; n++){
							data[j+n] = 0;
						}
					}
				}
			}
		}
	}

}

int main(int argc, char* argv[]){
    
    vector<string> f = get_file_name();

	char img_name[100];
	for (int no = 0; no < f.size(); ++no){

		sprintf(img_name, "天猫工商信息执照/%s", f[no].c_str());
		Mat img = imread(img_name);

		Mat img_binary, img_dilate;
		cvtColor(img, img_binary, COLOR_BGR2GRAY);
		threshold(img_binary, img_binary, 50, 255, THRESH_TOZERO_INV);
		threshold(img_binary, img_binary, 0, 255, THRESH_BINARY);

		Mat kernel = getStructuringElement(MORPH_RECT, Size(3,3), Point(-1,-1));
		dilate(img_binary, img_dilate, kernel, Point(-1,-1), 3);

		vector<vector<Point> > contours;
		findContours(img_dilate, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		if (contours.size() == 0)
			continue;
		for (int i = 0; i < contours.size(); ++i){
			Rect rect = boundingRect(contours[i]);

			if (rect.width > 25 && rect.tl().x > 30 && rect.tl().y < 30){
				Mat image_cut = Mat(img_binary, rect);
				cut_number(image_cut);
			}
			
			if (rect.width > 100 && rect.tl().x > 30 && rect.tl().y > 30 && rect.tl().y < 60 && rect.height < 40){
				Mat image_cut = Mat(img_binary, rect);
				pre_cut_bracket(image_cut);
				cut_word(image_cut);
			}
		}

	}

	return 0;
}