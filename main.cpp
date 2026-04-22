#include <bits/stdc++.h>
using namespace std;

struct Student {
    string name;
    char gender; // 'M' or 'F'
    int cls;
    int score[9]{};
};

static inline int avg_score(const Student &st) {
    long long sum = 0;
    for (int i = 0; i < 9; ++i) sum += st.score[i];
    return int(sum / 9);
}

struct Comparator {
    const unordered_map<string, Student>* mp;
    Comparator(const unordered_map<string, Student>* m=nullptr): mp(m) {}
    bool operator()(const string &a, const string &b) const {
        if (a == b) return false;
        const Student &sa = mp->at(a);
        const Student &sb = mp->at(b);
        int aa = avg_score(sa), bb = avg_score(sb);
        if (aa != bb) return aa > bb; // higher first
        for (int i = 0; i < 9; ++i) {
            if (sa.score[i] != sb.score[i]) return sa.score[i] > sb.score[i];
        }
        return a < b; // name lexicographically ascending
    }
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    unordered_map<string, Student> students;
    students.reserve(20000);
    vector<string> rank_order; // names in current ranking order
    unordered_map<string,int> rank_pos; // name -> 1-based ranking
    bool started = false;

    auto rebuild_positions = [&](){
        rank_pos.clear();
        rank_pos.reserve(rank_order.size()*2+1);
        for (int i = 0; i < (int)rank_order.size(); ++i) rank_pos[rank_order[i]] = i+1;
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
            for (int i = 0; i < 9; ++i) s.score[i] = sc[i];
            students.emplace(name, std::move(s));
        } else if (cmd == "START") {
            if (!started) {
                started = true;
                // initialize ranking
                rank_order.clear();
                rank_order.reserve(students.size());
                for (auto &p : students) rank_order.push_back(p.first);
                Comparator comp(&students);
                sort(rank_order.begin(), rank_order.end(), comp);
                rebuild_positions();
            }
        } else if (cmd == "UPDATE") {
            string name; int code, score;
            cin >> name >> code >> score;
            auto it = students.find(name);
            if (it == students.end()) {
                cout << "[Error]Update failed." << '\n';
                continue;
            }
            if (code >= 0 && code < 9) it->second.score[code] = score;
        } else if (cmd == "FLUSH") {
            if (started) {
                Comparator comp(&students);
                sort(rank_order.begin(), rank_order.end(), comp);
                rebuild_positions();
            } else {
                // Before START, FLUSH shouldn't appear per problem guarantee; ignore.
            }
        } else if (cmd == "PRINTLIST") {
            int n = (int)rank_order.size();
            for (int i = 0; i < n; ++i) {
                const Student &st = students.at(rank_order[i]);
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
            auto pit = rank_pos.find(name);
            if (pit != rank_pos.end()) pos = pit->second;
            else {
                // Fallback if not started yet: build ranking now
                rank_order.clear();
                for (auto &p : students) rank_order.push_back(p.first);
                Comparator comp(&students);
                sort(rank_order.begin(), rank_order.end(), comp);
                rebuild_positions();
                pos = rank_pos[name];
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
