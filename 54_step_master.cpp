#include<cstdio>
#include<cstdlib>
#include<algorithm>
#include<iostream>
#include<vector>
//#include <athread.h>
#include "mpi.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;

extern "C" {
#include <athread.h>

   void slave_func(void);
}

unsigned int m[16],x[70],y[70],mm[16],xx[70],yy[70],ivm[16],ivx[70],ivy[70],iv[10];
const unsigned int pr[4][32]={{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,5,14,7,0,9,2,11,4,13,6,15,8,1,10,3,12},
			   {7,4,13,1,10,6,15,3,12,0,9,5,2,14,11,8,6,11,3,7,0,13,5,10,14,15,8,12,4,9,1,2},
			   {3,10,14,4,9,15,8,1,2,7,0,6,13,11,5,12,15,5,1,3,7,14,6,9,11,8,12,2,10,0,4,13},
			   {1,9,11,10,0,8,12,4,13,3,7,15,14,5,6,2,8,6,4,1,3,11,15,0,5,12,2,13,9,7,10,14}};
const unsigned int s[4][32]={{11,14,15,12,5,8,7,9,11,13,14,15,6,7,9,8,8,9,9,11,13,15,15,5,7,7,8,11,14,14,12,6},
			   {7,6,8,13,11,9,7,15,7,12,15,9,11,7,13,12,9,13,15,7,12,8,9,11,7,7,12,7,6,15,13,11},
			   {11,13,6,7,14,9,13,15,14,8,13,6,5,12,7,5,9,7,15,11,8,6,6,14,12,13,5,14,13,13,7,5},
{11,12,14,15,14,15,9,8,9,14,5,6,8,6,5,12,15,5,8,11,14,14,6,14,6,9,12,9,12,5,15,8}};		

unsigned long long Z[2];
inline __attribute__((always_inline)) unsigned rnd()
{
	unsigned long long temp = Z[1] ^ (Z[1] << 63) ^ (Z[0] >> 1);
	Z[0] = Z[1];
	Z[1] = temp;
	return (unsigned)Z[0];
}
	   
inline __attribute__((always_inline)) void gety(int i)
{
	y[i+4]=((y[i]+((y[i+1]&y[i+3])^((~y[i+1])&y[i+2]))+m[pr[0][i+16]]+0x50a28be6)<<s[0][i+16])|((y[i]+((y[i+1]&y[i+3])^((~y[i+1])&y[i+2]))+m[pr[0][i+16]]+0x50a28be6)>>(32-s[0][i+16]));
}
inline __attribute__((always_inline)) void getyy(int i)
{
	yy[i+4]=((yy[i]+((yy[i+1]&yy[i+3])^((~yy[i+1])&yy[i+2]))+mm[pr[0][i+16]]+0x50a28be6)<<s[0][i+16])|((yy[i]+((yy[i+1]&yy[i+3])^((~yy[i+1])&yy[i+2]))+mm[pr[0][i+16]]+0x50a28be6)>>(32-s[0][i+16]));
}
inline __attribute__((always_inline)) void getx(int i)
{
	x[i+4]=((x[i]+(x[i+1]^x[i+2]^x[i+3])+m[pr[0][i]])<<s[0][i])|((x[i]+(x[i+1]^x[i+2]^x[i+3])+m[pr[0][i]])>>(32-s[0][i]));
}
inline __attribute__((always_inline)) void getxx(int i)
{
	xx[i+4]=((xx[i]+(xx[i+1]^xx[i+2]^xx[i+3])+mm[pr[0][i]])<<s[0][i])|((xx[i]+(xx[i+1]^xx[i+2]^xx[i+3])+mm[pr[0][i]])>>(32-s[0][i]));
}
inline __attribute__((always_inline)) void pry(int i)
{
		printf("%d %08x %08x %08x %08x %08x %08x %08x %08x\n",i+1,x[i+4],y[i+4],xx[i+4],yy[i+4],x[i+4]^xx[i+4],x[i+4]-xx[i+4],y[i+4]^yy[i+4],y[i+4]-yy[i+4]);
		for(int j=0;j<=31;j++)
		{
			if(((yy[i+4]&(1u<<j))!=0)&&((y[i+4]&(1u<<j))==0))
				printf("+%d ",j+1);
			if(((y[i+4]&(1u<<j))!=0)&&((yy[i+4]&(1u<<j))==0))
				printf("-%d ",j+1);
		}
		cout<<endl;
}		
inline __attribute__((always_inline)) void prx(int i)
{
	printf("%d %08x %08x %08x %08x %08x %08x %08x %08x\n",i+1,x[i+4],y[i+4],xx[i+4],yy[i+4],x[i+4]^xx[i+4],x[i+4]-xx[i+4],y[i+4]^yy[i+4],y[i+4]-yy[i+4]);
	for(int j=0;j<=31;j++)
	{
		if(((xx[i+4]&(1u<<j))!=0)&&((x[i+4]&(1u<<j))==0))
			printf("+%d ",j+1);
		if(((x[i+4]&(1u<<j))!=0)&&((xx[i+4]&(1u<<j))==0))
			printf("-%d ",j+1);
	}
	cout<<endl;
}
inline __attribute__((always_inline)) void getm(int i)
{
	m[pr[0][i+16]]=((y[i+4]>>s[0][i+16])|(y[i+4]<<(32-s[0][i+16])))-y[i]-((y[i+1]&y[i+3])^((~y[i+1])&y[i+2]))-0x50a28be6;
}
inline __attribute__((always_inline)) void getmx(int i)
{
	m[pr[0][i]]=((x[i+4]>>s[0][i])|(x[i+4]<<(32-s[0][i])))-x[i]-(x[i+1]^x[i+2]^x[i+3]);
}
unsigned pre[((1u<<22)+10)*5];
int bf=0,af=0;
vector<unsigned> vi[(1<<26)+10];
int mycid, numprocs;
int have[1000];
int st[70];
unsigned slav_m[70][20];
inline __attribute__((always_inline)) int getpre()
{
	unsigned uu=0;
	while(1)
	{
		m[8]=uu;
		x[8]=((x[12]>>s[0][8])|(x[12]<<(32-s[0][8])))-m[8]-(x[9]^x[10]^x[11]);
		x[7]=((x[11]>>s[0][7])|(x[11]<<(32-s[0][7])))-m[7]-(x[9]^x[10]^x[8]);
		unsigned tmp=x[8]^x[7];
		have[tmp%(1<<6)]=1;
		uu++;
		if(uu==0)
			break;
	}
	int now=0;
	for(unsigned i=0;i<(1u<<6);i++)
	{
		if(have[i]==0)
			continue;
		for(unsigned j=0;j<(1u<<26);j++)
		{
			vector<unsigned> ().swap(vi[j]);
		}
		unsigned u=0,w=0;
		while(1)
		{
			m[1]=u;
			getx(1);
			getx(2);
			unsigned tmp=(((x[9]>>s[0][5])|(x[9]<<(32-s[0][5])))-m[5]-x[5])^x[6];
			if((tmp%(1<<6))==i)
			{
				vi[tmp>>6].push_back(u);
				w++;
			}
			u++;
			if(u==0)
				break;
		}
		u=0;
		while(1)
		{
			m[8]=u;
			x[8]=((x[12]>>s[0][8])|(x[12]<<(32-s[0][8])))-m[8]-(x[9]^x[10]^x[11]);
			x[7]=((x[11]>>s[0][7])|(x[11]<<(32-s[0][7])))-m[7]-(x[9]^x[10]^x[8]);
			unsigned tmp=x[8]^x[7];
			if((tmp%(1<<6))==i)
				for(int j=0;j<vi[tmp>>6].size();j++)
				{
					m[1]=vi[tmp>>6][j];
					getx(1);
					getx(2);		
					getmx(3);
					getmx(4);
					getmx(6);
					gety(7);
					if((y[11]^y[10])&(1<<1))
						continue;
					if(y[11]&(1<<3))
						continue;
					if((y[11]&(1<<4))==0)
						continue;	
					if((y[11]&(1<<6))==0)
						continue;			
					if(y[11]&(1<<8))
						continue;	
					if((y[11]&(1<<17))==0)
						continue;	
					if(y[11]&(1<<19))
						continue;	
					if((y[11]&(1<<24))==0)
						continue;	
					if((y[11]&(1<<26))==0)
						continue;	
					if((y[11]&(1<<27))==0)
						continue;
					if(y[11]&(1<<29))
						continue;
					pre[now*5+0]=m[1];
					pre[now*5+1]=m[3];
					pre[now*5+2]=m[4];
					pre[now*5+3]=m[6];
					pre[now*5+4]=m[8];
					now++;
				}
			u++;
			if(u==0)
				break;
		}
	}		
	for(unsigned j=0;j<(1u<<26);j++)
	{
		vector<unsigned> ().swap(vi[j]);
	}
	return now;
}
int ed[70],numed=0;
int main(int argc, char *argv[])
{
	athread_init();
	sleep(10);
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &mycid);
	Z[0]=0x6ed9eba1e02b1f69llu;
	Z[1]=0x5c4dd1248f1bbcdcllu;
	Z[1]+=time(0)+mycid*998244353ll;
	ivx[0]=ivy[0]=0x67452301;
	ivx[1]=ivy[1]=0x10325476;
	ivx[2]=ivy[2]=0x98BADCFE;	
	ivx[3]=ivy[3]=0xEFCDAB89;	
	for(int i=0;i<16;i++)
		ivm[i]=rnd();
	for(int i=0;i<64;i++)
	{
		if(i<16)
		{
				ivx[i+4]=((ivx[i]+(ivx[i+1]^ivx[i+2]^ivx[i+3])+ivm[pr[0][i]])<<s[0][i])|((ivx[i]+(ivx[i+1]^ivx[i+2]^ivx[i+3])+ivm[pr[0][i]])>>(32-s[0][i]));
			ivy[i+4]=((ivy[i]+((ivy[i+1]&ivy[i+3])^((~ivy[i+1])&ivy[i+2]))+ivm[pr[0][i+16]]+0x50a28be6)<<s[0][i+16])|((ivy[i]+((ivy[i+1]&ivy[i+3])^((~ivy[i+1])&ivy[i+2]))+ivm[pr[0][i+16]]+0x50a28be6)>>(32-s[0][i+16]));
		}
		else
			if(i<32)
			{
				ivx[i+4]=((ivx[i]+((ivx[i+3]&ivx[i+2])^((~ivx[i+3])&ivx[i+1]))+ivm[pr[1][i-16]]+0x5a827999)<<s[1][i-16])|((ivx[i]+((ivx[i+3]&ivx[i+2])^((~ivx[i+3])&ivx[i+1]))+ivm[pr[1][i-16]]+0x5a827999)>>(32-s[1][i-16]));
				ivy[i+4]=((ivy[i]+(ivy[i+1]^((~ivy[i+2])|ivy[i+3]))+ivm[pr[1][i]]+0x5c4dd124)<<s[1][i])|((ivy[i]+(ivy[i+1]^((~ivy[i+2])|ivy[i+3]))+ivm[pr[1][i]]+0x5c4dd124)>>(32-s[1][i]));
			}
			else
				if(i<48)
				{
					ivx[i+4]=((ivx[i]+(ivx[i+1]^((~ivx[i+2])|ivx[i+3]))+ivm[pr[2][i-32]]+0x6ed9eba1)<<s[2][i-32])|((ivx[i]+(ivx[i+1]^((~ivx[i+2])|ivx[i+3]))+ivm[pr[2][i-32]]+0x6ed9eba1)>>(32-s[2][i-32]));
					ivy[i+4]=((ivy[i]+((ivy[i+3]&ivy[i+2])^((~ivy[i+3])&ivy[i+1]))+ivm[pr[2][i-16]]+0x6d703ef3)<<s[2][i-16])|((ivy[i]+((ivy[i+3]&ivy[i+2])^((~ivy[i+3])&ivy[i+1]))+ivm[pr[2][i-16]]+0x6d703ef3)>>(32-s[2][i-16]));
				}
				else
				{
					ivx[i+4]=((ivx[i]+((ivx[i+1]&ivx[i+3])^((~ivx[i+1])&ivx[i+2]))+ivm[pr[3][i-48]]+0x8f1bbcdc)<<s[3][i-48])|((ivx[i]+((ivx[i+1]&ivx[i+3])^((~ivx[i+1])&ivx[i+2]))+ivm[pr[3][i-48]]+0x8f1bbcdc)>>(32-s[3][i-48]));
					ivy[i+4]=((ivy[i]+(ivy[i+3]^ivy[i+2]^ivy[i+1])+ivm[pr[3][i-32]])<<s[3][i-32])|((ivy[i]+(ivy[i+3]^ivy[i+2]^ivy[i+1])+ivm[pr[3][i-32]])>>(32-s[3][i-32]));
				}
	}
	iv[0]=ivx[3]+ivx[56]+ivy[55];
	iv[1]=ivx[2]+ivx[55]+ivy[54];
	iv[2]=ivx[1]+ivx[54]+ivy[57];
	iv[3]=ivx[0]+ivx[57]+ivy[56];
	x[0]=y[0]=xx[0]=yy[0]=iv[0];
	x[1]=y[1]=xx[1]=yy[1]=iv[3];
	x[2]=y[2]=xx[2]=yy[2]=iv[2];
	x[3]=y[3]=xx[3]=yy[3]=iv[1];
	x[9]=xx[9]=0x2567d3d9;
	x[10]=xx[10]=0xc56b2f66;
	x[11]=0xfffe09ff;
	xx[11]=0x0001f800;
	x[12]=0x0faf2169;
	xx[12]=0xe02b1f69;
	x[13]=0x03bd206d;
	xx[13]=0xfbc11e6d;
	x[14]=0xe7eafa97;
	xx[14]=0xe3ed0497;
	x[15]=0x3b2bfdf7;
	getmx(11);
	getmx(10);
	getmx(9);
	while(1)
	{
		int r;
		r=rnd()&0x080001f7;
		x[14]^=r;
		xx[14]^=r;
		r=rnd()&0x038001f7;
		x[15]^=r;
		r=rnd()&0xe0000000;
		x[14]^=r;
		xx[14]^=r;
		x[15]^=r;
		getmx(11);
		mm[11]=m[11]-(1u<<17);
		getxx(11);
		getmx(10);
		mm[10]=m[10];
		getmx(9);
		mm[9]=m[9];
		y[4]=0xc003cdf9;
		getm(0);
		mm[5]=m[5];
		getyy(0);
		y[5]=0x2fffd17f;
		getm(1);
		mm[14]=m[14];
		getyy(1);
		y[6]=0xfffe6aff;
		getm(2);
		mm[7]=m[7]+(1u<<9)-(1u<<3);
		getyy(2);
		y[7]=0x08d38e34;
		getm(3);
		mm[0]=m[0];
		getyy(3);
		gety(4);
		getyy(4);
		int cc=0;
		for(int i=0;i<(1u<<18);i++)
		{
			y[9]=(rnd()&(0xfafff1bf))|0x280ac002;		
			getm(5);
			mm[2]=m[2];
			getyy(5);
			gety(6);
			getyy(6);				
			if(y[10]&(1<<4))
				continue;	
			if((y[10]&(1<<6))==0)
				continue;
			if((y[10]&(1<<9))==0)
				continue;
			if(y[10]&(1<<11))
				continue;
			if(y[10]&(1<<15))
				continue;
			if((y[10]&(1<<17))==0)
				continue;
			if(y[10]&(1<<19))
				continue;
			if((y[10]^y[9])&(1<<20))
				continue;	
			if(y[10]&(1<<24))
				continue;	
			if((y[10]&(1<<26))==0)
				continue;					
			if((y[10]&(1<<27))==0)
				continue;
			if(y[10]&(1<<29))
				continue;	
			cc=1;
			break;
		}
		if(cc==1)
			break;
	}
	getx(0);
	int num=getpre();
	cout<<num<<endl;
	__real_athread_spawn((void *)(slave_func), (void *)num);
	while(1)
	{
		for (int i = 0; i < 64; i++)
			if(st[i]==1)
			{
				for(int v=0;v<16;v++)
					printf("ivm:%d %08x\n",v,ivm[v]);
				for(int v=0;v<4;v++)
					printf("iv:%d %08x\n",v,iv[v]);
				for(int v=0;v<16;v++)
					printf("m:%d %08x\n",v,slav_m[i][v]);
				st[i]=0;
			}
			else
				if(st[i]==2)
				{
					if(ed[i]==0)
					{
						ed[i]=1;
						numed++;
					}
				}
		if(numed==64)
			break;
	}
	athread_join();
	athread_halt();
	MPI_Finalize();
}
