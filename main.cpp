#include <bits/stdc++.h>
using namespace std;

struct Student {
    string name;
    char gender; // 'M' or 'F'
    int cls;
    int score[9]{};
    int sum = 0;
    int avg = 0;
};

static inline int avg_score(const Student &st) {
    return st.avg;
}

struct RankKey {
    string name;
    int avg;
    int s[9];
};

static inline void build_rank_keys(const unordered_map<string, Student>& students,
                                   vector<RankKey>& keys) {
    keys.clear();
    keys.reserve(students.size());
    for (const auto &p : students) {
        RankKey rk;
        rk.name = p.first;
        rk.avg = p.second.avg;
        for (int i = 0; i < 9; ++i) rk.s[i] = p.second.score[i];
        keys.emplace_back(std::move(rk));
    }
}

static inline void sort_keys(vector<RankKey>& keys) {
    sort(keys.begin(), keys.end(), [](const RankKey& a, const RankKey& b){
        if (a.avg != b.avg) return a.avg > b.avg;
        for (int i = 0; i < 9; ++i) if (a.s[i] != b.s[i]) return a.s[i] > b.s[i];
        return a.name < b.name;
    });
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    unordered_map<string, Student> students;
    students.reserve(20000);
    vector<const Student*> rank_order; // students in current ranking order
    unordered_map<const Student*,int> rank_pos; // student ptr -> 1-based ranking
    bool started = false;

    auto rebuild_positions = [&](){
        rank_pos.clear();
        rank_pos.reserve(rank_order.size()*2+1);
        for (int i = 0; i < (int)rank_order.size(); ++i) rank_pos[rank_order[i]] = i+1;
    };

    auto rebuild_ranking = [&](){
        vector<RankKey> keys;
        build_rank_keys(students, keys);
        sort_keys(keys);
        rank_order.clear();
        rank_order.reserve(keys.size());
        for (auto &k : keys) {
            auto it = students.find(k.name);
            if (it != students.end()) rank_order.push_back(&it->second);
        }
        rebuild_positions();
    };

    string cmd;
    while (cin >> cmd) {
        if (cmd == "ADD") {
            string name; char gender; int cls; int sc[9];
            cin >> name >> gender >> cls;
            for (int i = 0; i < 9; ++i) cin >> sc[i];
            if (started) {
                cout << "[Error]Cannot add student now." << '\n';
                continue;
            }
            if (students.count(name)) {
                cout << "[Error]Add failed." << '\n';
                continue;
            }
            Student s; s.name = name; s.gender = gender; s.cls = cls;
            for (int i = 0; i < 9; ++i) { s.score[i] = sc[i]; s.sum += sc[i]; }
            s.avg = s.sum / 9;
            students.emplace(name, std::move(s));
        } else if (cmd == "START") {
            if (!started) {
                started = true;
                rebuild_ranking();
            }
        } else if (cmd == "UPDATE") {
            string name; int code, score;
            cin >> name >> code >> score;
            auto it = students.find(name);
            if (it == students.end()) {
                cout << "[Error]Update failed." << '\n';
                continue;
            }
            if (code >= 0 && code < 9) {
                it->second.sum += (score - it->second.score[code]);
                it->second.score[code] = score;
                it->second.avg = it->second.sum / 9;
            }
        } else if (cmd == "FLUSH") {
            if (started) rebuild_ranking();
        } else if (cmd == "PRINTLIST") {
            int n = (int)rank_order.size();
            for (int i = 0; i < n; ++i) {
                const Student &st = *rank_order[i];
                cout << (i+1) << ' ' << st.name << ' ' << (st.gender=='M'?"male":"female")
                     << ' ' << st.cls << ' ' << avg_score(st) << '\n';
            }
        } else if (cmd == "QUERY") {
            string name; cin >> name;
            auto it = students.find(name);
            if (it == students.end()) {
                cout << "[Error]Query failed." << '\n';
                continue;
            }
            int pos = 0;
            auto pit = rank_pos.find(&it->second);
            if (pit != rank_pos.end()) pos = pit->second;
            else {
                rebuild_ranking();
                pos = rank_pos[&it->second];
            }
            cout << "STUDENT " << name << " NOW AT RANKING " << pos << '\n';
        } else if (cmd == "END") {
            break;
        } else {
            string line; getline(cin, line);
        }
    }
    return 0;
}
