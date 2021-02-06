#include <stdio.h>
#include <iostream>
#include "16807.h"
using namespace std;


// ����/��������
constexpr auto PEOPLE = 20;
constexpr auto FIRE = 100;

// RANGE��ʾ���ķ�Χ����λ��km��
constexpr auto RANGE_L = 1;
constexpr auto RANGE_H = 10;



struct region
{
	double x;
	double y;
	double w;//weight��sum=1��
	double rho;//�뾶
	int num_of_fireman;
	int size;
};

region fire_region[FIRE];
double fire[FIRE][2];//���x,y
double fireman[PEOPLE][2];//����Աx,y



void setfire_start(double lambda_num, double lambda_size);
void recalc();
void setfire_firemen();
void setfire_write_file();
int Possion_sample(double lambda);
int JieCheng(int i);
double Exp_sample(double lambda);
double random();
int Discreate_sample(double weight[], int num);
double Norm_sample(double x, double sigma);

int main()
{
	// 1.�ڰ뾶Ϊ10km�ķ�Χ�ڳ������õ�����
	// 2.����ÿ�����㣬�����õ�����С
	// 3.��������С�������õ�������Ա��λ��
	setfire_start(3,10);//para1:������lambda(��Ӧ��̫��)��para2:����lambda(ע�������Ŀ��ֵΪpara1*para2)
	recalc();//�������еĻ�㣬���¼����������������ʹ�С
	setfire_firemen();
	setfire_write_file();

	return 0;
}

void setfire_start(double lambda_region, double lambda_size)
{
	// ��һ��:�ҳ�������Ŀ
	int fire_region_num;
	int fire_spot_num;
	for (int i=1;;++i)
	{
		fire_region_num = Possion_sample(lambda_region);
		if (fire_region_num >= FIRE)
		{
			printf("\n\n\n\t\t\tERROR!!!There are %d fire spots!\n\n\n", fire_region_num);
			continue;
		}
		if (fire_region_num)break;
		cout <<i<< "-th trial:"<<"Warning: fire_region_num=0, another initiation process will be done."<< endl;
	} 

	// �ڶ���:�ҳ�����λ��
	for (int i = 0; i < fire_region_num; ++i)
	{
		double r, theta;
		for (;;)
		{
			r = RANGE_H * sqrt(random());
			if (r > RANGE_L)break;
		}
		theta = 2 * PI * random();
		// �����õ�������x,y����
		fire_region[i].x = r*cos(theta);
		fire_region[i].y = abs(r * sin(theta));
		fire_region[i].w = Exp_sample(lambda_size);
	}
	// ����õ�������Ȩ��
	double temp_count = 0;
	for (int i = 0; i < fire_region_num; ++i)
	{
		temp_count += fire_region[i].w;
	}
	for (int i = 0; i < fire_region_num; ++i)
	{
		fire_region[i].w = fire_region[i].w / temp_count;
		fire_region[i].size++;
	}
	// �ɻ�����Ȩ�ؽ��г������õ�������size��Ҳ����ÿ�������ж��ٸ���㣩
	// �ȼ��������Ŀ��������ĿԽ�࣬�����Ŀ������Խ�ࣩ
	fire_spot_num = Possion_sample(fire_region_num * lambda_size);
	double weight[FIRE];
	for (int i = 0; i < fire_region_num; ++i)
	{
		weight[i]= fire_region[i].w;
	}
	for (int i = 0; i < fire_spot_num; ++i)
	{
		int temp = Discreate_sample(weight, fire_region_num);
		fire_region[temp - 1].size++;
	}

	// ������:�����õ�����x,y����
	int fire_spot_count=0;
	for (int i = 0; i < fire_region_num; ++i)
	{// ����ÿ��������˵
		for (int j = 0; j < fire_region[i].size; ++j)
		{// �������������ÿ�������˵
			// ȷ������λ��
			double r, theta;
			double rho = Norm_sample(pow(fire_region[i].size,0.3),0.1);// ���ֲ��İ뾶ȡ���ڻ����Ŀ����ĿԽ�࣬�뾶Խ��
			fire_region[i].rho = rho;
			r = rho * sqrt(random());
			if (r > rho) { cout << "ERROR!!!!!" << endl; getchar(); getchar(); }
			theta = 2 * PI * random();
			// �����õ�������x,y����
			fire[fire_spot_count][0] = r * cos(theta) + fire_region[i].x;
			fire[fire_spot_count][1] = r * sin(theta) + fire_region[i].y;
			fire_spot_count++;
		}
	}
}

void setfire_firemen()
{
	// ��һ�������ݻ����Ĵ�Сȷ����������ָ������Ա�ĸ���
	double region_radii_weight[FIRE];
	double region_radii_weight_sum = 0;
	for (int i = 0; i < FIRE; ++i)
	{
		region_radii_weight[i] = fire_region[i].size;
		region_radii_weight_sum += region_radii_weight[i];
	}
	for (int i = 0; i < FIRE; ++i)
	{
		region_radii_weight[i] = region_radii_weight[i] / region_radii_weight_sum;
	}
	int region_num = 0;// �ж��ٸ�����
	for (int i = 0; i < FIRE; ++i)
	{
		if (fire_region[i].size != 0)
		{
			region_num++;
			fire_region[i].num_of_fireman++;
		}
	}
	if (region_num > PEOPLE) { cout << "ERROR!!! region number is bigger than total people" << endl; }
	for (int i = 0; i < PEOPLE - region_num; ++i)
	{
		int man_in_region = Discreate_sample(region_radii_weight,FIRE);
		fire_region[man_in_region-1].num_of_fireman++;
	}

	// �ڶ�����ָ������Ա��λ��
	int man_count = 0;
	for (int i = 0; i < FIRE; i++)
	{
		double x = fire_region[i].x;
		double y = fire_region[i].y;
		double theta0 = atan(y/x);
		if (x > 0)theta0 += PI;
		double range_of_theta = 2;
		double r, theta;
		double rho = Norm_sample(pow(fire_region[i].size, 0.3), 0.1);// ���ֲ��İ뾶ȡ���ڻ����Ŀ����ĿԽ�࣬�뾶Խ��
		

		for (int j = 0; j < fire_region[i].num_of_fireman; j++)
		{
			r = sqrt(random());
			r = fire_region[i].rho + 0.1 * r * fire_region[i].rho;
			//r = 0;
			theta = (theta0 + range_of_theta * (random() - 0.5));
			fireman[man_count][0] = x + r * cos(theta);
			fireman[man_count][1] = y + r * sin(theta);
			man_count++;
		}
	}
	// cout << man_count << endl;
	// getchar();
}
void setfire_write_file()
{
	FILE* f_fire = fopen("fire.dat", "w");
	FILE* f_fireman = fopen("fireman.dat", "w");
	FILE* f_region = fopen("fire_region.dat", "w");
	FILE* f_for_zw = fopen("fire_zw.dat", "w");

	int region_num = 0;// �ж��ٸ�����
	int fire_num = 0;// �ж��ٸ����
	for (int i = 0; i < FIRE; ++i)
	{
		if (fire_region[i].size != 0)
		{
			region_num++;
			fire_num += fire_region[i].size;
		}
	}

	
	for (int i = 0; i < fire_num; ++i)
	{
		fprintf(f_fire, "%6f\t%6f\n", fire[i][0], fire[i][1]);
	}
	for (int i = 0; i < PEOPLE; ++i)
	{
		fprintf(f_fireman, "%6f\t%6f\n", fireman[i][0], fireman[i][1]);
	}
	for (int i = 0; i < FIRE; ++i)
	{
		fprintf(f_region, "%6f\t%6f\t%6f\n", fire_region[i].x, fire_region[i].y, fire_region[i].rho);
	}
	int spot_count = 0;
	for (int i = 0; i < FIRE; ++i)
	{
		if (fire_region[i].size == 0)continue;
		fprintf(f_for_zw,"%6f\t%6f\t%6d\n", fire_region[i].x, fire_region[i].y, fire_region[i].size);
		// fprintf(f_for_zw, "%6f\t%6f\t%6d\t%d\n", fire_region[i].x, fire_region[i].y, fire_region[i].size, fire_region[i].num_of_fireman);
		for (int j = 0; j < fire_region[i].size; ++j)
		{
			fprintf(f_for_zw, "%6f\t%6f\n", fire[spot_count][0], fire[spot_count][1]);
			spot_count++;
		}
	}
	fclose(f_fire);
	fclose(f_fireman);
}

int Possion_sample(double lambda)
// �������õ����ɷֲ�
{
	// p��0-1�����
	double p = random();
	int k = 0;
	double count = 0;
	for (count = 0; ; k++)
	{
		count += pow(lambda, k) / JieCheng(k) * exp(-lambda);
		if (count > p)break;
	}
	return k;
}

double Exp_sample(double lambda)
// �������õ�ָ���ֲ�
{
	double xi = random();
	return -lambda * log(xi);
}

int JieCheng(int i)
// ����i�Ľ׳�
{
	int result = 1;
	if (i < 0) { printf("ERROR in JieCheng();(�׳˼�������)��\n"); getchar(); return -1; }
	if (i == 0)return 1;
	for (;; i--)
	{
		result *= i;
		if (i == 1)break;
	}
	return result;
}

double random()
{
	return random_16807();
}

int Discreate_sample(double weight[],int num)
// ����weight()Ȩ��������ȡ��ɢ������
// ����ֵ�ǳ�ȡ���ı�����Ҳ�����ڵڼ�������У�
// num��weight�����鳤��
{
	double count=0;
	double x = random();
	for (int i=0;i<num;++i)
	{
		if (x > count)count += weight[i];
		else return i;
	}
	return num;
}

double Norm_sample(double x, double sigma)
{
	double xi1 = random();
	double xi2 = random();//xi1 xi2��uniform distribution
	double X = pow(-2 * log(xi1), 0.5) * cos(2 * PI * xi2);
	return X * sigma + x;
}

void recalc()
{// �������еĻ�㣬���¼����������������ʹ�С
	int fire_spot_count = 0;
	for (int i = 0; i < FIRE; ++i)
	{
		if (fire_region[i].size == 0)continue;
		// ��������
		double x_sum = 0;
		double y_sum = 0;
		int temp_spot_count = fire_spot_count;
		for (int j = 0; j < fire_region[i].size; j++)
		{
			x_sum += fire[temp_spot_count][0];
			y_sum += fire[temp_spot_count][1];
			temp_spot_count++;
		}
		for (int j = 0; j < fire_region[i].size; j++)
		{
			fire_region[i].x = x_sum / fire_region[i].size;
			fire_region[i].y = y_sum / fire_region[i].size;
		}
		// ��С
		double max = 0;
		for (int j = 0; j < fire_region[i].size; j++)
		{
			double temp = sqrt((fire[fire_spot_count][0] - fire_region[i].x) *
				(fire[fire_spot_count][0] - fire_region[i].x) +
				(fire[fire_spot_count][1] - fire_region[i].y) *
				(fire[fire_spot_count][1] - fire_region[i].y));
			if (temp > max)max = temp;
			fire_spot_count++;
		}
		fire_region[i].rho = max;
	}
}






