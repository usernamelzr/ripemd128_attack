#include <stdio.h>
#include <math.h>
#include <string.h>
#include "slave.h"


__thread_local volatile unsigned long get_reply,put_reply;
__thread_local volatile unsigned long start,end;

__thread_local int my_id;

extern unsigned pre[((1u<<22)+10)*5];

extern int st[70];

extern unsigned slav_m[70][20];


__thread_local const unsigned int pr_s[4][32]={{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,5,14,7,0,9,2,11,4,13,6,15,8,1,10,3,12},
			   {7,4,13,1,10,6,15,3,12,0,9,5,2,14,11,8,6,11,3,7,0,13,5,10,14,15,8,12,4,9,1,2},
			   {3,10,14,4,9,15,8,1,2,7,0,6,13,11,5,12,15,5,1,3,7,14,6,9,11,8,12,2,10,0,4,13},
			   {1,9,11,10,0,8,12,4,13,3,7,15,14,5,6,2,8,6,4,1,3,11,15,0,5,12,2,13,9,7,10,14}};
			   
__thread_local const unsigned int s_s[4][32]={{11,14,15,12,5,8,7,9,11,13,14,15,6,7,9,8,8,9,9,11,13,15,15,5,7,7,8,11,14,14,12,6},
			   {7,6,8,13,11,9,7,15,7,12,15,9,11,7,13,12,9,13,15,7,12,8,9,11,7,7,12,7,6,15,13,11},
			   {11,13,6,7,14,9,13,15,14,8,13,6,5,12,7,5,9,7,15,11,8,6,6,14,12,13,5,14,13,13,7,5},
			   {11,12,14,15,14,15,9,8,9,14,5,6,8,6,5,12,15,5,8,11,14,14,6,14,6,9,12,9,12,5,15,8}};	

__thread_local unsigned m_s[16],x_s[70],y_s[70],mm_s[16],xx_s[70],yy_s[70];

__thread_local unsigned pre_slave[10010];

inline __attribute__((always_inline)) void gety_s(int i)
{
	y_s[i+4]=((y_s[i]+((y_s[i+1]&y_s[i+3])^((~y_s[i+1])&y_s[i+2]))+m_s[pr_s[0][i+16]]+0x50a28be6)<<s_s[0][i+16])|((y_s[i]+((y_s[i+1]&y_s[i+3])^((~y_s[i+1])&y_s[i+2]))+m_s[pr_s[0][i+16]]+0x50a28be6)>>(32-s_s[0][i+16]));
}
inline __attribute__((always_inline)) void getyy_s(int i)
{
	yy_s[i+4]=((yy_s[i]+((yy_s[i+1]&yy_s[i+3])^((~yy_s[i+1])&yy_s[i+2]))+mm_s[pr_s[0][i+16]]+0x50a28be6)<<s_s[0][i+16])|((yy_s[i]+((yy_s[i+1]&yy_s[i+3])^((~yy_s[i+1])&yy_s[i+2]))+mm_s[pr_s[0][i+16]]+0x50a28be6)>>(32-s_s[0][i+16]));
}
inline __attribute__((always_inline)) void getx_s(int i)
{
	x_s[i+4]=((x_s[i]+(x_s[i+1]^x_s[i+2]^x_s[i+3])+m_s[pr_s[0][i]])<<s_s[0][i])|((x_s[i]+(x_s[i+1]^x_s[i+2]^x_s[i+3])+m_s[pr_s[0][i]])>>(32-s_s[0][i]));
}
inline __attribute__((always_inline)) void getxx_s(int i)
{
	xx_s[i+4]=((xx_s[i]+(xx_s[i+1]^xx_s[i+2]^xx_s[i+3])+mm_s[pr_s[0][i]])<<s_s[0][i])|((xx_s[i]+(xx_s[i+1]^xx_s[i+2]^xx_s[i+3])+mm_s[pr_s[0][i]])>>(32-s_s[0][i]));
}      
inline __attribute__((always_inline)) void getm_s(int i)
{
	m_s[pr_s[0][i+16]]=((y_s[i+4]>>s_s[0][i+16])|(y_s[i+4]<<(32-s_s[0][i+16])))-y_s[i]-((y_s[i+1]&y_s[i+3])^((~y_s[i+1])&y_s[i+2]))-0x50a28be6;
}
inline __attribute__((always_inline)) void getmx_s(int i)
{
	m_s[pr_s[0][i]]=((x_s[i+4]>>s_s[0][i])|(x_s[i+4]<<(32-s_s[0][i])))-x_s[i]-(x_s[i+1]^x_s[i+2]^x_s[i+3]);
}	  

extern unsigned int m[16],x[70],y[70],mm[16],xx[70],yy[70],iv[10];

void func(void* sum)
{
	m_s[5]=m[5];
	m_s[14]=m[14];
	m_s[7]=m[7];
	m_s[0]=m[0];
	m_s[2]=m[2];
	m_s[9]=m[9];
	m_s[10]=m[10];	
	m_s[11]=m[11];
	mm_s[5]=mm[5];
	mm_s[14]=mm[14];
	mm_s[7]=mm[7];
	mm_s[0]=mm[0];
	mm_s[2]=mm[2];
	mm_s[9]=mm[9];
	mm_s[10]=mm[10];	
	mm_s[11]=mm[11];
	int num=(int)sum;
	my_id = athread_get_id(-1);
	int my_start=num*my_id/64,my_end=num*(my_id+1)/64;
	int now_start=my_start,now_end=my_start;
	int i,v,len;
	unsigned j,k,u;
	for(i=9;i<=15;i++)
	{
		x_s[i]=x[i];
		xx_s[i]=xx[i];
	}
	x_s[0]=y_s[0]=xx_s[0]=yy_s[0]=iv[0];
	x_s[1]=y_s[1]=xx_s[1]=yy_s[1]=iv[3];
	x_s[2]=y_s[2]=xx_s[2]=yy_s[2]=iv[2];
	x_s[3]=y_s[3]=xx_s[3]=yy_s[3]=iv[1];
	x_s[4]=x[4];
	for(i=4;i<=10;i++)
	{
		y_s[i]=y[i];
		yy_s[i]=yy[i];
	}
	for(i=my_start;i<my_end;i++)
	{
		if(i>=now_end)
		{
			now_start=now_end;
			get_reply = 0;
			if(now_end+2000>=my_end)
			{
				len=(my_end-now_end)*20;
				now_end=my_end;
			}
			else
			{
				len=40000;
				now_end+=2000;
			}
			athread_get(PE_MODE,&pre[now_start*5],&pre_slave[0],len,&get_reply,0,0,0);
			while(get_reply!=1);
		}
		m_s[1]=mm_s[1]=pre_slave[(i-now_start)*5+0];
		m_s[3]=mm_s[3]=pre_slave[(i-now_start)*5+1];
		m_s[4]=mm_s[4]=pre_slave[(i-now_start)*5+2];
		m_s[6]=mm_s[6]=pre_slave[(i-now_start)*5+3];
		m_s[8]=mm_s[8]=pre_slave[(i-now_start)*5+4];
		gety_s(7);
		for(j=0x92796ca7;j!=0;j=((j-1)&0x92796ca7))
		{
			y_s[12]=(j&(0xdf7dffef))|0x4d041148;
			if((y_s[12]&(1<<9))!=(y_s[11]&(1<<9)))
				y_s[12]^=(1<<9);
			if((y_s[12]&(1<<15))!=(y_s[11]&(1<<15)))
				y_s[12]^=(1<<15);
			getm_s(8);
			mm_s[13]=m_s[13];
			gety_s(9);
			if(y_s[13]&(1<<3))
				continue;
			if((y_s[13]^y_s[12])&(1<<6))
				continue;			
			if((y_s[13]&(1<<8))==0)
				continue;
			if(y_s[13]&(1<<15))
				continue;
			if((y_s[13]&(1<<17))==0)
				continue;	
			if((y_s[13]^y_s[12])&(1<<19))
				continue;	
			if(y_s[13]&(1<<21))
				continue;	
			if((y_s[13]&(1<<23))==0)
				continue;
			if((y_s[13]^y_s[12])&(1<<24))
				continue;	
			if(y_s[13]&(1<<29))
				continue;
			for(k=0x92596ca7;k;k=(0x92596ca7&(k-1)))
			{
				y_s[14]=(k&(0xff7ffcff))|0x20020008;
				if((y_s[14]&(1<<27))!=(y_s[13]&(1<<27)))
					y_s[14]^=(1<<27);	
				if((y_s[14]&(1<<4))!=(y_s[13]&(1<<4)))
					y_s[14]^=(1<<4);
				getm_s(10);
				mm_s[15]=m_s[15]+(1u<<26);
				gety_s(11);
				gety_s(12);
				if((y_s[15]&(1<<3))==0)
					y_s[14]^=(1<<24);
				if(y_s[15]&(1<<5))
					y_s[14]^=(1<<26);	
				if((y_s[15]&(1<<9))==0)
					y_s[14]^=(1<<30);
				if((y_s[15]&(1<<17))==0)
					y_s[14]^=(1<<6);	
				if((y_s[15]&(1<<23))==0)
					y_s[14]^=(1<<12);	
				if(y_s[15]&(1<<29))
					y_s[14]^=(1<<18);	
				if(y_s[16]&(1<<3))
					y_s[14]^=(1<<21);	
				if((y_s[16]&(1<<29))==0)
					y_s[14]^=(1<<15);	
				getm_s(10);
				mm_s[15]=m_s[15]+(1u<<26);
				gety_s(11);
				gety_s(12);
				gety_s(13);
				gety_s(14);
				if((y_s[15]&(1<<3))==0)
					continue;
				if(y_s[15]&(1<<5))
					continue;	
				if(y_s[15]&(1<<8))
					continue;	
				if((y_s[15]&(1<<9))==0)
					continue;
				if((y_s[15]&(1<<17))==0)
					continue;	
				if((y_s[15]&(1<<23))==0)
					continue;	
				if(y_s[15]&(1<<29))
					continue;	
				if(y_s[16]&(1<<3))
					continue;
				if((y_s[16]&(1<<5))==0)
					continue;		
				if((y_s[16]&(1<<9))==0)
					continue;
				if(y_s[16]&(1<<17))
					continue;	
				if(y_s[16]&(1<<23))
					continue;	
				if((y_s[16]&(1<<29))==0)
					continue;	
				if(y_s[17]&(1<<3))
					continue;
				if((y_s[17]&(1<<5))==0)
					continue;	
				if((y_s[17]^y_s[16])&(1<<8))
					continue;		
				if(y_s[17]&(1<<9))
					continue;	
				if(y_s[17]&(1<<17))
					continue;	
				if(y_s[17]&(1<<29))
					continue;	
				if(y_s[18]&(1<<3))
					continue;
				if((y_s[18]&(1<<5))==0)
					continue;	
				if((y_s[18]&(1<<9))==0)
					continue;
				if(y_s[18]&(1<<14))
					continue;
				if((y_s[18]&(1<<17))==0)
					continue;
				for(u=0xe3e800d0;u!=0;u=((u-1)&0xe3e800d0))
				{
					x_s[16]=(u&0xfffffdf7)|0x00040400;
					if(((x_s[16]^x_s[14])&(1<<27))==0)
						x_s[16]^=(1<<27);
					getmx_s(12);
					mm_s[12]=m_s[12];
					gety_s(15);
					if(y_s[19]&(1<<3))
					{
						if(((x_s[16]^y_s[19])&(1<<2))==0)
							continue;
						else
							x_s[16]^=(1<<2);
					}
					getx_s(13);
					getx_s(14);
					if(((x_s[18]^x_s[17])&(1<<9))==0)
						x_s[16]^=(1<<0);
					getx_s(13);
					getx_s(14);
					if((x_s[18]^x_s[17])&(1<<10))
						x_s[16]^=(1<<1);
					getmx_s(12);
					mm_s[12]=m_s[12];
					gety_s(15);
					if(y_s[19]&(1<<5))
						x_s[16]^=(1<<5);
					if((y_s[19]&(1<<9))==0)
					{
						if(((x_s[16]^y_s[19])&(1<<8))==0)
							continue;
						else
							x_s[16]^=(1<<8);
					}
					getx_s(13);
					if(((x_s[17]^x_s[16])&(1<<11))==0)
						x_s[16]^=(1<<11);
					if(((x_s[17]^x_s[16])&(1<<12))==0)
						x_s[16]^=(1<<12);
					if(((x_s[17]^x_s[16])&(1<<13))==0)
						x_s[16]^=(1<<13);
					getmx_s(12);
					gety_s(15);				
					if((y_s[19]&(1<<14))==0)
						x_s[16]^=(1<<14);	
					getx_s(13);	
					if(((x_s[17]^x_s[16])&(1<<14))==0)
						continue;
					if(((x_s[17]^x_s[16])&(1<<15))==0)
						x_s[16]^=(1<<15);		
					if((x_s[17]^x_s[16])&(1<<16))
						x_s[16]^=(1<<16);	
					getmx_s(12);
					gety_s(15);				
					if((y_s[19]&(1<<17))==0)
						x_s[16]^=(1<<17);
					if(y_s[19]&(1<<18))
						continue;
					getx_s(13);
					if((x_s[17]^x_s[16])&(1<<17))
						continue;
					if(((x_s[17]^x_s[16])&(1<<26))==0)
						x_s[16]^=(1<<26);	
					if(((x_s[17]^x_s[16])&(1<<27))==0)
						x_s[16]^=(1<<20);	
					getx_s(13);
					getx_s(14);
					if(((x_s[18]^x_s[17])&(1<<3))==0)
						x_s[16]^=(1<<28);
					getmx_s(12);
					mm_s[12]=m_s[12];
					for(v=9;v<16;v++)
						getxx_s(v);
					for(v=13;v<16;v++)
						getx_s(v);
					if(x_s[19]!=xx_s[19])
						continue;
					gety_s(15);
					for(v=0;v<16;v++)
						getyy_s(v);
					for(v=16;v<26;v++)
					{
						y_s[v+4]=((y_s[v]+(y_s[v+1]^((~y_s[v+2])|y_s[v+3]))+m_s[pr_s[1][v]]+0x5c4dd124)<<s_s[1][v])|((y_s[v]+(y_s[v+1]^((~y_s[v+2])|y_s[v+3]))+m_s[pr_s[1][v]]+0x5c4dd124)>>(32-s_s[1][v]));
						yy_s[v+4]=((yy_s[v]+(yy_s[v+1]^((~yy_s[v+2])|yy_s[v+3]))+mm_s[pr_s[1][v]]+0x5c4dd124)<<s_s[1][v])|((yy_s[v]+(yy_s[v+1]^((~yy_s[v+2])|yy_s[v+3]))+mm_s[pr_s[1][v]]+0x5c4dd124)>>(32-s_s[1][v]));
					}
					if((y_s[26]!=yy_s[26])||(y_s[27]!=yy_s[27])||(y_s[28]!=yy_s[28])||(y_s[29]!=yy_s[29]))
						continue;
					for(v=26;v<32;v++)
					{
						y_s[v+4]=((y_s[v]+(y_s[v+1]^((~y_s[v+2])|y_s[v+3]))+m_s[pr_s[1][v]]+0x5c4dd124)<<s_s[1][v])|((y_s[v]+(y_s[v+1]^((~y_s[v+2])|y_s[v+3]))+m_s[pr_s[1][v]]+0x5c4dd124)>>(32-s_s[1][v]));
						yy_s[v+4]=((yy_s[v]+(yy_s[v+1]^((~yy_s[v+2])|yy_s[v+3]))+mm_s[pr_s[1][v]]+0x5c4dd124)<<s_s[1][v])|((yy_s[v]+(yy_s[v+1]^((~yy_s[v+2])|yy_s[v+3]))+mm_s[pr_s[1][v]]+0x5c4dd124)>>(32-s_s[1][v]));
					}
					for(v=16;v<32;v++)
					{
						x_s[v+4]=((x_s[v]+((x_s[v+3]&x_s[v+2])^((~x_s[v+3])&x_s[v+1]))+m_s[pr_s[1][v-16]]+0x5a827999)<<s_s[1][v-16])|((x_s[v]+((x_s[v+3]&x_s[v+2])^((~x_s[v+3])&x_s[v+1]))+m_s[pr_s[1][v-16]]+0x5a827999)>>(32-s_s[1][v-16]));
						xx_s[v+4]=((xx_s[v]+((xx_s[v+3]&xx_s[v+2])^((~xx_s[v+3])&xx_s[v+1]))+mm_s[pr_s[1][v-16]]+0x5a827999)<<s_s[1][v-16])|((xx_s[v]+((xx_s[v+3]&xx_s[v+2])^((~xx_s[v+3])&xx_s[v+1]))+mm_s[pr_s[1][v-16]]+0x5a827999)>>(32-s_s[1][v-16]));
					}
					/*if((x_s[32]==xx_s[32])&&(x_s[33]==xx_s[33])&&(x_s[34]==xx_s[34])&&(x_s[35]==xx_s[35])&&(y_s[32]==yy_s[32])&&(y_s[33]==yy_s[33])&&(y_s[34]==yy_s[34])&&(y_s[35]==yy_s[35]))
					{
						while(st[my_id]!=0);
						for(v=0;v<16;v++)
							slav_m[my_id][v]=m_s[v];
						st[my_id]=1;
					}	*/
 					for(v=32;v<48;v++)
					{
						x_s[v+4]=((x_s[v]+(x_s[v+1]^((~x_s[v+2])|x_s[v+3]))+m_s[pr_s[2][v-32]]+0x6ed9eba1)<<s_s[2][v-32])|((x_s[v]+(x_s[v+1]^((~x_s[v+2])|x_s[v+3]))+m_s[pr_s[2][v-32]]+0x6ed9eba1)>>(32-s_s[2][v-32]));
						y_s[v+4]=((y_s[v]+((y_s[v+3]&y_s[v+2])^((~y_s[v+3])&y_s[v+1]))+m_s[pr_s[2][v-16]]+0x6d703ef3)<<s_s[2][v-16])|((y_s[v]+((y_s[v+3]&y_s[v+2])^((~y_s[v+3])&y_s[v+1]))+m_s[pr_s[2][v-16]]+0x6d703ef3)>>(32-s_s[2][v-16]));
						xx_s[v+4]=((xx_s[v]+(xx_s[v+1]^((~xx_s[v+2])|xx_s[v+3]))+mm_s[pr_s[2][v-32]]+0x6ed9eba1)<<s_s[2][v-32])|((xx_s[v]+(xx_s[v+1]^((~xx_s[v+2])|xx_s[v+3]))+mm_s[pr_s[2][v-32]]+0x6ed9eba1)>>(32-s_s[2][v-32]));
						yy_s[v+4]=((yy_s[v]+((yy_s[v+3]&yy_s[v+2])^((~yy_s[v+3])&yy_s[v+1]))+mm_s[pr_s[2][v-16]]+0x6d703ef3)<<s_s[2][v-16])|((yy_s[v]+((yy_s[v+3]&yy_s[v+2])^((~yy_s[v+3])&yy_s[v+1]))+mm_s[pr_s[2][v-16]]+0x6d703ef3)>>(32-s_s[2][v-16]));
					}
					for(v=48;v<54;v++)
					{
						x_s[v+4]=((x_s[v]+((x_s[v+1]&x_s[v+3])^((~x_s[v+1])&x_s[v+2]))+m_s[pr_s[3][v-48]]+0x8f1bbcdc)<<s_s[3][v-48])|((x_s[v]+((x_s[v+1]&x_s[v+3])^((~x_s[v+1])&x_s[v+2]))+m_s[pr_s[3][v-48]]+0x8f1bbcdc)>>(32-s_s[3][v-48]));
						y_s[v+4]=((y_s[v]+(y_s[v+3]^y_s[v+2]^y_s[v+1])+m_s[pr_s[3][v-32]])<<s_s[3][v-32])|((y_s[v]+(y_s[v+3]^y_s[v+2]^y_s[v+1])+m_s[pr_s[3][v-32]])>>(32-s_s[3][v-32]));
						xx_s[v+4]=((xx_s[v]+((xx_s[v+1]&xx_s[v+3])^((~xx_s[v+1])&xx_s[v+2]))+mm_s[pr_s[3][v-48]]+0x8f1bbcdc)<<s_s[3][v-48])|((xx_s[v]+((xx_s[v+1]&xx_s[v+3])^((~xx_s[v+1])&xx_s[v+2]))+mm_s[pr_s[3][v-48]]+0x8f1bbcdc)>>(32-s_s[3][v-48]));
						yy_s[v+4]=((yy_s[v]+(yy_s[v+3]^yy_s[v+2]^yy_s[v+1])+mm_s[pr_s[3][v-32]])<<s_s[3][v-32])|((yy_s[v]+(yy_s[v+3]^yy_s[v+2]^yy_s[v+1])+mm_s[pr_s[3][v-32]])>>(32-s_s[3][v-32]));											
					}
					if(((x_s[54]+y_s[57])==(xx_s[54]+yy_s[57]))&&((x_s[55]+y_s[54])==(xx_s[55]+yy_s[54]))&&((x_s[56]+y_s[55])==(xx_s[56]+yy_s[55]))&&((x_s[57]+y_s[56])==(xx_s[57]+yy_s[56])))
					{
						while(st[my_id]!=0);
						for(v=0;v<16;v++)
							slav_m[my_id][v]=m_s[v];
						st[my_id]=1;
					}	 	
				} 
			}
		}
	}
	while(st[my_id]!=0);
	st[my_id]=2;
}
