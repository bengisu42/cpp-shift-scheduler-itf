#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <ctime>
#include <iomanip>

using namespace std;

struct Participant
{
    string name;
    string iban;
    string bank;
    string tc;
    string availability;
    int assignedShifts = 0; // Katılımcının kaç kere atandığı
};

struct Shift
{
    string date;
    int capacity;
    vector<string> assignedParticipants;
};

// Veriyi tırnak içindeki alanlara göre okuyacak yardımcı fonksiyon
string readQuotedField(stringstream& ss)
{
    string field;
    char ch;
    bool insideQuotes = false;

    while (ss >> noskipws >> ch)
    {
        if (ch == '"')
        {
            insideQuotes = !insideQuotes;
        }
        else if (ch == ',' && !insideQuotes)
        {
            break;
        }
        else
        {
            field += ch;
        }
    }
    return field;
}
map<string, int> getShiftRates()
{
    return
    {
     {"22 Ekim", 550},
    {"23 Ekim Alan", 400},
    {"23 Ekim Uniq", 500},
    {"24 Ekim Alan", 400},
    {"26 Ekim", 400},
    {"27 Ekim", 400},
    {"31 Ekim Muhsin", 750},
    {"2 Kasim Arter", 400},
    {"2 Kasim Muhsin", 500},
    {"3 Kasim Arter", 650},
    {"3 Kasim Muhsin", 500},
    {"4 Kasim", 400},
    {"5 Kasim", 400},
    {"6 Kasim", 400},
    {"7 Kasim DasDas", 350},
    {"7 Kasim Tekel", 400},
    {"8 Kasim Mon", 400},
    {"8 Kasim DasDas", 650},
    {"9 Kasim Mon", 650},
    {"9 Kasim Tekel", 400},
    {"10 Kasim", 400},
    {"12 Kasim Alan", 400},
    {"12 Kasim Crr", 450},
    {"13 Kasim Alan", 400},
    {"13 Kasim Crr", 450},
    {"13 Kasim Tekel", 400},
    {"14 Kasim", 400},
    {"16 Kasim Tekel", 400},
    {"16 Kasim Alan", 400},
    {"17 Kasim", 400}

    };



}
// Katılımcıların ücretlerini hesaplayan fonksiyon
void calculatePayments(const map<string, Shift>& shiftPlan, const vector<Participant>& participants)
{
    // Gün bazında ücret tablosu
    map<string, int> dailyRates =
    {
        {"10 Kasim", 400}, {"12 Kasim Alan", 400}, {"12 Kasim Crr", 450}, {"13 Kasim Alan", 400},
        {"13 Kasim Crr", 450}, {"13 Kasim Tekel", 400}, {"14 Kasim", 400}, {"16 Kasim Alan", 400},
        {"16 Kasim Tekel", 400}, {"17 Kasim", 400}, {"2 Kasim Arter", 400}, {"2 Kasim Muhsin", 500},
        {"22 Ekim", 550}, {"23 Ekim Alan", 400}, {"23 Ekim Uniq", 500}, {"24 Ekim Alan", 400},
        {"26 Ekim", 400}, {"27 Ekim", 400}, {"3 Kasim Arter", 650}, {"3 Kasim Muhsin", 500},
        {"31 Ekim Muhsin", 750}, {"4 Kasim", 400}, {"5 Kasim", 400}, {"6 Kasim", 400},
        {"7 Kasim DasDas", 350}, {"7 Kasim Tekel", 400}, {"8 Kasim DasDas", 400},
        {"8 Kasim Mon", 650}, {"9 Kasim Mon", 650}, {"9 Kasim Tekel", 400}
    };

    // Katılımcıya göre ücretleri toplamak için harita
    map<string, int> paymentSummary;

    // Şift planını dolaşarak ücretleri hesapla
    for (const auto& shift : shiftPlan)
    {
        // Günün ücretini al
        int rate = dailyRates[shift.first];

        for (const auto& participantName : shift.second.assignedParticipants)
        {
            paymentSummary[participantName] += rate;
        }
    }

    // Ödeme dökümünü IBAN bilgileri ile yazdır
    cout << "\nKatilimcilarin Odemeleri:\n";
    for (const auto& entry : paymentSummary)
    {
        // Katılımcının bilgilerini bul
        auto it = find_if(participants.begin(), participants.end(), [&](const Participant& p) { return p.name == entry.first; });

        if (it != participants.end())
        {
            cout << "TC: " << it->tc << ", Isim: " << it->name
                << ", IBAN: " << it->iban
                << ", Odenecek Ucret: " << entry.second << " TL\n";
        }
    }
}


// Katılımcıların uygun olduğu günleri bulma fonksiyonu
bool isParticipantAvailable(const Participant& p, const string& shiftDay)
{
    return p.availability.find(shiftDay) != string::npos;
}

// Shift planında bir kişiyi değiştir
void changeShiftPerson(map<string, Shift>& shiftPlan, const string& day, const string& oldPerson, const string& newPerson, vector<Participant>& participants)
{
    if (shiftPlan.find(day) != shiftPlan.end())
    {
        vector<string>& participantsList = shiftPlan[day].assignedParticipants;

        // Vardiyadaki eski katılımcıyı yeni katılımcı ile değiştir
        auto it = find(participantsList.begin(), participantsList.end(), oldPerson);
        if (it != participantsList.end())
        {
            *it = newPerson; // Eski katılımcıyı yeni katılımcı ile değiştir
            cout << oldPerson << " yerine " << newPerson << " atandi.\n";

            // Eski katılımcının çalışma sayısını azalt, yeni katılımcının sayısını artır
            auto oldIt = find_if(participants.begin(), participants.end(), [&](const Participant& p) { return p.name == oldPerson; });
            if (oldIt != participants.end()) oldIt->assignedShifts--;

            auto newIt = find_if(participants.begin(), participants.end(), [&](const Participant& p) { return p.name == newPerson; });
            if (newIt != participants.end()) newIt->assignedShifts++;

        }
        else
        {
            cout << oldPerson << " bu gunde calismiyor.\n";
        }
    }
    else
    {
        cout << "Bu gun icin bir shift bulunamadi.\n";
    }
}

// Çalışma günleri sayısını güncelle
void updateWorkDaysCount(map<string, int>& workDays, const map<string, Shift>& shiftPlan)
{
    workDays.clear(); // Önceki verileri temizle
    for (const auto& entry : shiftPlan)
    {
        for (const auto& person : entry.second.assignedParticipants)
        {
            workDays[person]++;
        }
    }
}

// Çalışma günlerini yazdır
void printWorkDays(const map<string, int>& workDays)
{
    cout << "\nKatilimcilarin calisma Gunleri:\n";
    for (const auto& entry : workDays)
    {
        cout << entry.first << " " << entry.second << " gun calisti.\n";
    }
}

// Belirli günlerde çakışmayı kontrol eden fonksiyon
bool isSpecificShiftConflict(const string& shift1, const string& shift2)
{
    vector<pair<string, string>> conflictDays =
    {
        {"2 Kasim Arter", "2 Kasim Muhsin"},
        {"3 Kasim Arter", "3 Kasim Muhsin"},
        {"7 Kasim DasDas", "7 Kasim Tekel"},
        {"8 Kasim Mon", "8 Kasim DasDas"},
        {"9 Kasim Mon", "9 Kasim Tekel"},
        {"12 Kasim Alan", "12 Kasim Crr"},
        {"13 Kasim Alan", "13 Kasim Crr"},
        {"13 Kasim Alan", "13 Kasim Tekel"},
        {"13 Kasim Crr", "13 Kasim Tekel"},
        {"16 Kasim Tekel", "16 Kasim Alan"}
    };

    for (const auto& conflictPair : conflictDays)
    {
        if ((shift1 == conflictPair.first && shift2 == conflictPair.second) ||
            (shift1 == conflictPair.second && shift2 == conflictPair.first))
        {
            return true;
        }
    }
    return false;
}
// Güncellenmiş shift planını tablo formatında yazdırma fonksiyonu
void printUpdatedShiftPlan(const map<string, Shift>& shiftPlan)
{
    cout << "\nGuncellenmis Shift Plani:\n";

    // Başlıkları hizalı şekilde yazdır
    cout << left << setw(20) << "Tarih"
        << left << setw(10) << "Kapasite"
        << "Katilimcilar" << endl;
    cout << string(60, '-') << endl; // Bölme çizgisi

    for (const auto& entry : shiftPlan)
    {
        // Tarih ve kapasiteyi hizalı şekilde yazdır
        cout << left << setw(20) << entry.first
            << left << setw(10) << entry.second.capacity;

        // Katılımcıları yazdır, aralarına virgül koy
        for (size_t i = 0; i < entry.second.assignedParticipants.size(); ++i)
        {
            cout << entry.second.assignedParticipants[i];
            if (i < entry.second.assignedParticipants.size() - 1)
            {
                cout << ", ";  // Son katılımcıdan sonra virgül koyma
            }
        }
        cout << endl;
    }
}
// Katılımcının çakışmalı bir güne atanıp atanmadığını kontrol etme fonksiyonu
bool isParticipantAssignedOnSpecificConflict(const Participant& p, const string& shiftDay, const map<string, Shift>& shiftPlan)
{
    for (const auto& entry : shiftPlan)
    {
        if (isSpecificShiftConflict(shiftDay, entry.first))
        {
            if (find(entry.second.assignedParticipants.begin(), entry.second.assignedParticipants.end(), p.name) != entry.second.assignedParticipants.end())
            {
                return true; // Çakışan şiftlerde çalışıyor
            }
        }
    }
    return false;
}

// Her katılımcıyı en az iki kere shift'e atamak
void assignMinimumShifts(vector<Participant>& participants, map<string, Shift>& shiftPlan)
{
    srand(time(0)); // Rastgeleliği başlat

    for (auto& participant : participants)
    {
        vector<string> availableShifts;

        // Katılımcının uygun olduğu günleri bul
        for (const auto& shift : shiftPlan)
        {
            if (isParticipantAvailable(participant, shift.first) && !isParticipantAssignedOnSpecificConflict(participant, shift.first, shiftPlan))
            {
                availableShifts.push_back(shift.first);
            }
        }

        // Eğer uygunluk iki günden fazlaysa, rastgele iki gün seç
        if (availableShifts.size() > 2)
        {
            random_shuffle(availableShifts.begin(), availableShifts.end());
            availableShifts.resize(2);  // İlk iki tanesini seç
        }

        // Seçilen günlere katılımcıyı atama
        for (const auto& day : availableShifts)
        {
            if (shiftPlan[day].assignedParticipants.size() < shiftPlan[day].capacity && find(shiftPlan[day].assignedParticipants.begin(), shiftPlan[day].assignedParticipants.end(), participant.name) == shiftPlan[day].assignedParticipants.end())
            {
                {
                    shiftPlan[day].assignedParticipants.push_back(participant.name);
                    participant.assignedShifts++;
                }
            }
        }
    }
}

void updatePaymentSummary(map<string, int>& paymentSummary, const map<string, Shift>& shiftPlan, const vector<Participant>& participants)
{
    // Günlük ücret tablosunu güncelleme
    map<string, int> dailyRates = getShiftRates();

    // Önceki ödeme bilgilerini sıfırla
    paymentSummary.clear();

    // Şift planını dolaşarak ücretleri güncelle
    for (const auto& shift : shiftPlan)
    {
        int rate = dailyRates[shift.first];
        for (const auto& participantName : shift.second.assignedParticipants)
        {
            paymentSummary[participantName] += rate;
        }
    }
}

// Güncellenmiş ödemeleri yazdırma fonksiyonu
void printPaymentSummary(const map<string, int>& paymentSummary, const vector<Participant>& participants)
{
    cout << "\nKatilimcilarin Odemeleri:\n";
    for (const auto& entry : paymentSummary)
    {
        auto it = find_if(participants.begin(), participants.end(), [&](const Participant& p) { return p.name == entry.first; });

        if (it != participants.end())
        {
            cout << "TC: " << it->tc << ", Isim: " << it->name
                << ", IBAN: " << it->iban
                << ", Odenecek Ucret: " << entry.second << " TL\n";
        }
    }
}


void printShiftPlanAndWorkDays(const map<string, Shift>& shiftPlan, const map<string, int>& workDays)
{
    cout << "\nGuncellenmis Shift Plani:\n";
    // Başlıkları hizalı şekilde yazdır
    cout << left << setw(20) << "Tarih"
        << left << setw(10) << "Kapasite"
        << "Katilimcilar" << endl;
    cout << string(60, '-') << endl; // Bölme çizgisi

    for (const auto& entry : shiftPlan)
    {
        // Tarih ve kapasiteyi hizalı şekilde yazdır
        cout << left << setw(20) << entry.first
            << left << setw(10) << entry.second.capacity;

        // Katılımcıları yazdır, aralarına virgül koy
        for (size_t i = 0; i < entry.second.assignedParticipants.size(); ++i)
        {
            cout << entry.second.assignedParticipants[i];
            if (i < entry.second.assignedParticipants.size() - 1)
            {
                cout << ", ";  // Son katılımcıdan sonra virgül koyma
            }
        }
        cout << endl;
    }

    // Katılımcıların çalışma günleri sayısını güncelle ve yazdır
    cout << "\nGuncellenmis Katilimcilarin Calisma Gunleri:\n";
    for (const auto& entry : workDays)
    {
        cout << left << setw(20) << entry.first
            << left << setw(10) << entry.second << " gun calisti." << endl;
    }
}

int main()
{
    vector<Participant> participants;
    ifstream file("C:\\Users\\bengisu.karadag\\Downloads\\finalfinall.csv"); // Yüklenen CSV dosyası

    if (!file.is_open())
    {
        cerr << "Dosya acilamadi!" << endl;
        return 1;
    }

    string line;
    getline(file, line); // İlk satırı (başlık satırını) atla

    // Katılımcı verilerini oku
    while (getline(file, line))
    {
        if (line.empty()) continue; // Boş satırları atla

        stringstream ss(line);
        string name, iban, bank, tc, availability;

        name = readQuotedField(ss);
        iban = readQuotedField(ss);
        bank = readQuotedField(ss);
        tc = readQuotedField(ss);
        availability = readQuotedField(ss);

        Participant p = { name, iban, bank, tc, availability };
        participants.push_back(p);
    }

    file.close();

    // Günler ve Kapasiteleri (tarihler ve mekanlar)
    map<string, Shift> shiftPlan =
    {
        {"22 Ekim", {"22 Ekim", 5}},
        {"23 Ekim Alan", {"23 Ekim Alan", 4}},
        {"23 Ekim Uniq", {"23 Ekim Uniq", 5}},
        {"24 Ekim Alan", {"24 Ekim Alan", 4}},
        {"26 Ekim", {"26 Ekim", 3}},
        {"27 Ekim", {"27 Ekim", 5}},
        {"31 Ekim Muhsin", {"31 Ekim Muhsin", 4}},
        {"2 Kasim Arter", {"2 Kasim Arter", 3}},
        {"2 Kasim Muhsin", {"2 Kasim Muhsin", 4}},
        {"3 Kasim Arter", {"3 Kasim Arter", 3}},
        {"3 Kasim Muhsin", {"3 Kasim Muhsin", 8}},
        {"4 Kasim", {"4 Kasim", 3}},
        {"5 Kasim", {"5 Kasim", 3}},
        {"6 Kasim", {"6 Kasim", 4}},
        {"7 Kasim DasDas", {"7 Kasim DasDas", 5}},
        {"7 Kasim Tekel", {"7 Kasim Tekel", 4}},
        {"8 Kasim Mon", {"8 Kasim Mon", 6}},
        {"8 Kasim DasDas", {"8 Kasim DasDas", 5}},
        {"9 Kasim Mon", {"9 Kasim Mon", 6}},
        {"9 Kasim Tekel", {"9 Kasim Tekel", 4}},
        {"10 Kasim", {"10 Kasim", 4}},
        {"12 Kasim Alan", {"12 Kasim Alan", 3}},
        {"12 Kasim Crr", {"12 Kasim Crr", 10}},
        {"13 Kasim Alan", {"13 Kasim Alan", 3}},
        {"13 Kasim Crr", {"13 Kasim Crr", 10}},
        {"13 Kasim Tekel", {"13 Kasim Tekel", 4}},
            {"14 Kasim", {"14 Kasim", 4}},
    {"16 Kasim Tekel", {"16 Kasim Tekel", 4}},
    {"16 Kasim Alan", {"16 Kasim Alan", 3}},
    {"17 Kasim", {"17 Kasim", 3}},

    };

    // Herkesin en az bir kere atanmasını sağla
    for (auto& participant : participants)
    {
        bool assigned = false;
        for (auto& shift : shiftPlan)
        {
            if (isParticipantAvailable(participant, shift.first) && shift.second.assignedParticipants.size() < shift.second.capacity)
            {
                shift.second.assignedParticipants.push_back(participant.name);
                participant.assignedShifts++;
                assigned = true;
                break; // Her katılımcı bir kere atandıktan sonra çık
            }
        }
        if (!assigned)
        {
            cout << participant.name << " hicbir shifte atanamadi.\n"; // Atanamadıysa bildirim yap
        }
    }

    // Herkesin en az iki kere atanmasını sağla
    assignMinimumShifts(participants, shiftPlan);

    // Kalan kapasiteleri eşit şekilde dağıt
    srand(time(0)); // Random seçim için
    for (auto& shift : shiftPlan)
    {
        vector<Participant*> availableParticipants;

        // Uygun olan katılımcıları bul
        for (auto& participant : participants)
        {
            if (isParticipantAvailable(participant, shift.first) && !isParticipantAssignedOnSpecificConflict(participant, shift.first, shiftPlan)) {
                availableParticipants.push_back(&participant);
            }
        }

        // Katılımcıları shift sayısına göre sıralayın (en az çalışanlar öne geçer)
        sort(availableParticipants.begin(), availableParticipants.end(), [](Participant* a, Participant* b)
            {return a->assignedShifts < b->assignedShifts;});

        // Kalan kapasiteleri doldur
        for (auto& participant : availableParticipants)
        {
            if (shift.second.assignedParticipants.size() < shift.second.capacity && find(shift.second.assignedParticipants.begin(), shift.second.assignedParticipants.end(), participant->name) == shift.second.assignedParticipants.end())
            {
                shift.second.assignedParticipants.push_back(participant->name);
                participant->assignedShifts++;
            }
        }
    }

    // Çıktıyı yazdırma
    cout << "\nShift Plani:\n";

    // Başlıkları hizalı şekilde yazdır
    cout << left << setw(20) << "Tarih"
        << left << setw(10) << "Kapasite"
        << "Katilimcilar" << endl;
    cout << string(60, '-') << endl; // Bölme çizgisi

    for (const auto& entry : shiftPlan)
    {
        // Tarih ve kapasiteyi hizalı şekilde yazdır
        cout << left << setw(20) << entry.first
            << left << setw(10) << entry.second.capacity;

        // Katılımcıları yazdır, aralarına virgül koy
        for (size_t i = 0; i < entry.second.assignedParticipants.size(); ++i)
        {
            cout << entry.second.assignedParticipants[i];
            if (i < entry.second.assignedParticipants.size() - 1)
            {
                cout << ", ";  // Son katılımcıdan sonra virgül koyma
            }
        }
        cout << endl;
    }

    // Katılımcıların çalışma günleri sayısını güncelle
    map<string, int> workDays;
    updateWorkDaysCount(workDays, shiftPlan);

    // Katılımcıların çalışma günlerini yazdır
    printWorkDays(workDays);
    calculatePayments(shiftPlan, participants);

    // Shift planında değişiklik yapmak isteyip istemediğini sor
    char response;
    do
    {
        cout << "\nShift planinda bir kisiyi degistirmek ister misiniz? (E/H): ";
        cin >> response;

        if (response == 'E' || response == 'e')
        {
            string day, oldPerson, newPerson;

            cout << "Lutfen degistirme yapmak istediginiz gunu girin: ";
            cin.ignore(); // Önceki cin'den kalan newline'ı temizler
            getline(cin, day);

            cout << "Degistirmek istediginiz kisinin adini girin (Bu kisi " << day << " gunu calisiyor olmali): ";
            getline(cin, oldPerson);

            cout << "Yeni kisinin adini girin: ";
            getline(cin, newPerson);

            // Shift planında kişiyi değiştir
            changeShiftPerson(shiftPlan, day, oldPerson, newPerson, participants);

            map<string, int> workDays;
            map<string, int> paymentSummary;
            updateWorkDaysCount(workDays, shiftPlan);
            updatePaymentSummary(paymentSummary, shiftPlan, participants);
            printShiftPlanAndWorkDays(shiftPlan, workDays);
            printPaymentSummary(paymentSummary, participants);
        }

    } while (response != 'H' && response != 'h'); // 'H' veya 'h' girilmedikçe döngü devam eder

    cout << "\nDegisiklikler tamamlandi." << endl;

    return 0;
}