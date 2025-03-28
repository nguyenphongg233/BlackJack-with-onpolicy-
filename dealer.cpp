#include<bits/stdc++.h>

using namespace std;

#define read() ios_base::sync_with_stdio(0);cin.tie(0);cout.tie(0)
#define day() time_t now = time(0);char* x = ctime(&now);cerr<<"Right Now Is : "<<x<<"\n"

#define int long long
#define ii pair<int,int>
#define X first
#define Y second 

const long long MAX = (int)1e5 + 5;
const long long INF = (int)1e9;
const long long MOD = (int)1e9 + 7;
//const long long RAND_MAX = (int)1e9;

char card[] = {'A','2','3','4','5','6','7','8','9','T','T','T','T'};
int Rand(int l,int r){
	return rand() % (r - l + 1) + l;
}
double epsilon;
double policy[20][22][2];
int state_count[20][22][2][2];
double state_sum[20][22][2][2];
double state_value[20][22][2][2];

// dealer_first_card - player_current_sum - usable ace 
bool acee = 0;
struct BlackJack{
	vector<char> a;
	int t;
	void shuffle(){
		for(int step = 0; step < 100; step++){
			int u = Rand(0, 51);
			int v = Rand(0, 51);
			swap(a[u], a[v]);
		}
	}
	char draw_card(){
		return a[Rand(0,51)];
	}
	void setup(){
		for(int i = 0; i < 4; i++){
			for(int j = 0; j < 13; j++){
				a.push_back(card[j]);
			}
		}
		t = 0;
	}
	void newgame(){
		t = 0;
		shuffle();
	}
} f;

struct Hand{
	bool ace;
	bool burst;
	int point;
	Hand(){
		ace = false;
		burst = false;
		point = 0;
	}
	void add_card(char c){
		int value;
		if (c == 'A') {
			ace = true;
			value = 11;
		} else if (c == 'T') {
			value = 10;
		} else {
			value = c - '0';
		}
		point += value;
		if (point > 21 && ace) {
			point -= 10;
			ace = false;
		}
		if (point > 21) burst = true;
	}
};
int random_choice(double hit){
	double c = (rand() % RAND_MAX) * 1.0 / (double) RAND_MAX;
	return c <= hit;
}
vector<pair<pair<ii,ii>,double>> play_episode(int first_cardx = -1,int player_pointx = 0,int usable_ace = 0){
//	f.newgame();
	Hand player, dealer;
	
	player.add_card(f.draw_card());
	player.add_card(f.draw_card());
	dealer.add_card(f.draw_card());
	int first_card = dealer.point;
	dealer.add_card(f.draw_card());
	
	if(first_cardx != -1){
		first_card = first_cardx;
		player.point = player_pointx;
		player.ace = usable_ace;
	}
	
	vector<pair<pair<ii,ii>,double>> d;
	
	while(true){
		int action = random_choice(policy[first_card][player.point][player.ace]);
		if(player.point == 21){
			d.push_back({{{first_card,player.point},{player.ace,0}},1});
			break;
		}
		if(action == 0){
			while(dealer.point < 17){
				dealer.add_card(f.draw_card());
			}
			if(dealer.burst || player.point > dealer.point){
				d.push_back({{{first_card,player.point},{player.ace,action}},1});
			}else if(dealer.point > player.point){
				d.push_back({{{first_card,player.point},{player.ace,action}},-1});
//				if(player.point >= 20){
//					cout << player.point << " " << dealer.point << '\n';
//				}
			}else d.push_back({{{first_card,player.point},{player.ace,action}},0});
			break;
		}
		int lp = player.point;
		int la = player.ace;
		player.add_card(f.draw_card());
		
		if(player.burst){
			d.push_back({{{first_card,lp},{la,action}},-1});
			break;
		}else if(player.point == 21){
			d.push_back({{{first_card,lp},{la,action}},1});
			break;
		}else {
			d.push_back({{{first_card,lp},{la,action}},0});
		}
	}
	// reward : 0 -> chua burst hoac hoa dealer 
	// -1 : burst hoac thua 
	// +1 chi khi thang dealer 
	
	return d;
}
void play(int first_cardx = -1,int player_pointx = 0,int usable_ace = 0,double gamma = 0.1){
	vector<pair<pair<ii,ii>,double>> episode = play_episode(first_cardx,player_pointx,usable_ace);
	reverse(episode.begin(),episode.end());
	
	double G = 0;
	
	for(auto e : episode){
		int first_card = e.X.X.X;
		int player_point = e.X.X.Y;
		int player_ace = e.X.Y.X;
		int player_action = e.X.Y.Y;
		int reward = e.Y;
		
		G = gamma * G + 1.0 * reward;
		state_count[first_card][player_point][player_ace][player_action]++;
		state_sum[first_card][player_point][player_ace][player_action] += G;
		state_value[first_card][player_point][player_ace][player_action] = state_sum[first_card][player_point][player_ace][player_action] / state_count[first_card][player_point][player_ace][player_action];
		
		//cout << G << "\n";
		// vi trong blackjack nen moi state se chi xuat hien 1 lan -> kh can dk 
		
		if(state_value[first_card][player_point][player_ace][1] > state_value[first_card][player_point][player_ace][0]){
			policy[first_card][player_point][player_ace] = 1.0 - epsilon + epsilon / 2.0;
		}else{
			policy[first_card][player_point][player_ace] = epsilon / 2.0;
		}
	}
}
void on_policy(int t = 10000000){
	while(t--){
		//cout << t << '\n';
//		for(int first_CARD = 1;first_CARD < 12;first_CARD++){
//			for(int player_point = 12;player_point <= 21;player_point++){
//				play(first_CARD,player_point,1);
//			}
//		}
//		for(int first_CARD = 1;first_CARD < 12;first_CARD++){
//			for(int player_point = 2;player_point <= 21;player_point++){
//				play(first_CARD,player_point,0);
//			}
//		}
//		epsilon = epsilon * 0.995;
		play();
	}
	
}
void initial(){
	for(int dealer = 0;dealer <= 11;dealer++){
		for(int player = 0;player <= 21;player++){
			for(int i = 0;i < 2;i++){
				if(player < 17){
					policy[dealer][player][i] = 1.0 - epsilon + epsilon / 2.0;
					//cout << 1.0 - epsilon + epsilon / 2.0 << '\n';
				}else policy[dealer][player][i] = epsilon / 2.0;
			}
		}
	}
}


// check lai gameeeeeeeeeeee

signed main(){
	read();
	freopen("dealer.out","w",stdout);
	epsilon = 0.1;
	srand(time(0));
	f.setup();
	initial();
	on_policy();
	//pair<pair<ii,ii>,int> a = game();
	//cout << a.X.X.X << " " << a.X.X.Y << " " << a.X.Y.X << " " << a.X.Y.Y << " " << a.Y << '\n';
	cout << setprecision(6) << fixed;
	vector<int> a[4];
	
	for(int i = 0;i < 2;i++){
		vector<int> x;
		vector<int> y;
		
		for(int dealer = 1;dealer <= 11;dealer++){
			int last = 11;
			for(int player = 1;player <= 21;player++){
				if(policy[dealer][player][i] >= 0.5){
					last = player;
					x.push_back(dealer);
					y.push_back(player);
					
				}
			}
		}
		
		if(i == 0){
			a[0] = x;
			a[1] = y;
		}else{
			a[2] = x;
			a[3] = y;
		}
		
	}
	
	while(a[0].size() < a[2].size()){
		a[0].push_back(1);
		a[1].push_back(1);
	}
	
	
	while(a[0].size() > a[2].size()){
		a[2].push_back(1);
		a[3].push_back(1);
	}
	
	a[0].push_back(1);
	a[1].push_back(1);
	a[2].push_back(1);
	a[3].push_back(1);	
	
	for(int i = 0;i < a[0].size();i++){
		cout << a[0][i] << "," << a[1][i] << "," << a[2][i] << "," << a[3][i] << '\n';
	}
	
	
}
