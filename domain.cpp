#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <set>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>

using namespace std;

template <typename Number>
Number ReadNumberOnLine(istream& input);

class Domain {
public:
    explicit Domain(string_view str) : domain_(str.rbegin(), str.rend()) {
        domain_ += '.';
    }

    bool operator==(const Domain& other) const {
        return domain_ == other.domain_;
    }

    bool operator<(const Domain& other) const {
        return ranges::lexicographical_compare(domain_, other.domain_);
    }

    bool IsSubdomain(const Domain& other) const {
        if (other.domain_ == "") {
            return false;
        }

        return domain_.starts_with(other.domain_);
    }

    void Print() const {
        cout << domain_ << endl;
    }

    string GetName() const {
        return domain_;
    }

private:
    string domain_;
};

class DomainChecker {
public:
    template <typename Iterator>
    DomainChecker(Iterator begin, Iterator end) 
    : filtered_domains_(make_move_iterator(begin), make_move_iterator(end)) { 
        sort(filtered_domains_.begin(), filtered_domains_.end()); 

        auto it = unique(filtered_domains_.begin(), filtered_domains_.end(),
                        [](const Domain& lhs, const Domain& rhs) { return rhs.IsSubdomain(lhs); });

        filtered_domains_.erase(it, filtered_domains_.end());
    }

    bool IsForbidden(const Domain& domain) const {
        auto it = upper_bound(filtered_domains_.begin(), filtered_domains_.end(), domain);
        return it != filtered_domains_.begin() && domain.IsSubdomain(*(--it));
    }


    vector<string> GetDomainsNames() const {
        vector<string> res;
        for (const Domain& domain : filtered_domains_){
            res.push_back(domain.GetName());
        }
        return res;
    }

    void PrintDomains() const {
        for (const auto& dom : filtered_domains_){
            dom.Print();
        }
    }

private:
    vector<Domain> filtered_domains_;
};


// Function to read domains from input
vector<Domain> ReadDomains(istream& in, size_t count) {
    vector<Domain> domains;
    domains.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        string tmp;
        getline(in, tmp);
        domains.emplace_back(tmp);
    }
    return domains;
}

template <typename Number>
Number ReadNumberOnLine(istream& input) {
    string line;
    getline(input, line);

    Number num;
    istringstream(line) >> num;
    return num;
}

void TestSubDomain() {

    assert(Domain("prod.frod.mail.ru").IsSubdomain(Domain("ru")));
    assert(Domain("gdz.com").IsSubdomain(Domain("com")));
    assert(Domain("m.maps.me").IsSubdomain(Domain("maps.me")));
    assert(!Domain("aaa.bb").IsSubdomain(Domain("aaaa.b")));
    assert(!Domain("aaa.bb").IsSubdomain(Domain("aaa.bbb")));
    assert(!Domain("aaa.bb").IsSubdomain(Domain("aa.bb")));
    assert(!Domain("aaa.bb").IsSubdomain(Domain("aaaa.bbbb")));
    assert(Domain("prod.frod.mail.ru").IsSubdomain(Domain("mail.ru")));
    assert(Domain("prod.frod.mail.ru").IsSubdomain(Domain("frod.mail.ru")));
    assert(Domain("prod.frod.mail.ru").IsSubdomain(Domain("prod.frod.mail.ru")));
    assert(!Domain("prod.frod.mail.ru").IsSubdomain(Domain("neprod.frod.mail.ru")));
    assert(!Domain("prod.frod.mail.ru").IsSubdomain(Domain("test.frod.mail.ru")));
    assert(!Domain("prod.frod.mail.ru").IsSubdomain(Domain("gail.com")));
    assert(!Domain("prod.frod.mail.ru").IsSubdomain(Domain("")));
    assert(!Domain("prod.frod.mail.ru").IsSubdomain(Domain("                              ")));
}

void TestExcludeFobiddenSubDomains() {

    vector<Domain> raw_domains{Domain("mail.ru"), Domain("mail.ru"), Domain("ru"), Domain("com"), Domain("mail.mail.ru"), Domain("yandex.org")};

    DomainChecker test_checker(raw_domains.begin(), raw_domains.end());
    vector<string> result = {"gro.xednay.", "moc.","ur."};
    assert(test_checker.GetDomainsNames() == result);

}

int main() {
    TestSubDomain();
    TestExcludeFobiddenSubDomains();

    const vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());
    const vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain& domain : test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}
