/**
 * 用于我的世界(Minecraft)铁砧附魔最佳计算的工具
 * INPUT:输入包含若干组数据，每组数据的第一个是附魔个数n(1<n<11)，以下n个数是花费cost[i](cost[i]<12) ##你也不想爆__int32吧(
 * OUTPUT:太乱了，懒，第一行是结论，最小附魔总经验
 * 时间复杂度 上届是O(2^n*ans^2*log_ans)的 ans是最小附魔总经验
 * 理论依据：对n>1，总是不会出现，最优策略是或可以是，先集中所有的附魔书到一本，再打到工具上的情形
 */

#include <cstdio>
#include <cassert>
#include <vector>
#include <map>
#include <algorithm>

#define BITCOUNT(x) (((BX_(x)+(BX_(x)>>4)) & 0x0F0F0F0F) % 255) // 效率仅次于基于指令集的popcount !
#define BX_(x) ((x) - (((x)>>1)&0x77777777) - (((x)>>2)&0x33333333) - (((x)>>3)&0x11111111))
using namespace std;
typedef vector<pair<int, int> > History;
struct extra {
	int punish;
	History history;
	extra() {} ;
	extra(int punish) : punish(punish) {} ;
	extra(int punish, History history) : punish(punish), history(history) {};
	~extra() {
		history.clear();
		history.shrink_to_fit(); // C++11
	}
};
typedef map<int, extra, greater<int> > mmap;
const int k_maxn = 10;
int n;
int value[1<<k_maxn];
vector<int> a[k_maxn+1];
mmap dp[1<<k_maxn][2];
void update(mmap& out, const int lft, const mmap& ml, const int rt, const mmap& mr) {
	for (auto iterL = ml.rbegin(); iterL != ml.rend(); iterL++) {
		for (auto iterR = mr.rbegin(); iterR != mr.rend(); iterR++) {
			int xp = value[rt]+ iterL->first + iterL->second.punish + iterR->first + iterR->second.punish;
			int punish = ( max(iterL->second.punish, iterR->second.punish ) << 1 ) | 1;
			auto it = out.lower_bound(xp);
			if (it == out.end() || it->second.punish > punish) {
				out[xp] = extra(punish, iterL->second.history);
				auto tit = out.find(xp);
				tit->second.history.insert(tit->second.history.end(), iterR->second.history.begin(), iterR->second.history.end());
				tit->second.history.push_back(make_pair(lft, rt));
				while(tit != out.begin()) {
					--tit;
					if (tit->second.punish < punish) break;
					tit = out.erase(tit);
				} // UGLY
			}
		}
	}
}
int main() {
	while (~scanf("%d", &n) && n) {	
		for (int i = 0; i < (1<<k_maxn); i++) {
			dp[i][1].clear(); dp[i][0].clear(); // clear() 怎么不执行析构呀，内存泄露喽
    		}
		for (int i = 2; i <= n; i++) { a[i].clear(); }
		for (int i = 1; i < (1<<n); ++i) {
			if (BITCOUNT(i) == 1) {
				scanf("%d", value + i);
				dp[i][1][value[i]] = extra(1, History {make_pair(-1, i)});
				dp[i][0][0] = extra(0);
			} else a[BITCOUNT(i)].push_back(i);
		}
		for (int i = 2; i <= n; ++i) {
			for (int j : a[i]) {
				value[j] = value[j&(j-1)] + value[j&(-j)];
				for (int lft = j&(j-1); lft; lft = j&(lft-1)) {
					int rt = j & (~lft);
					update(dp[j][1], lft, dp[lft][1], rt, dp[rt][0]);
					update(dp[j][0], lft, dp[lft][0], rt, dp[rt][0]);
				}
			}
		}
		printf("%d\n", dp[(1<<n)-1][1].rbegin()->first);
		for (auto i : dp[(1<<n)-1][1].rbegin()->second.history) {
			printf("%d %d\n", i.first==-1?-1:value[i.first], value[i.second]);
		}
	}
}
