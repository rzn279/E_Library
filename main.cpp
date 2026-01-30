#include <iostream>          // untuk input dan output data
#include <cstdlib>           // untuk konversi data dan system clear
#include <fstream>           // untuk memanipulasi file
#include <string>            // untuk memanipulasi string
#include <vector>            // untuk menyimpan data dalam bentuk array dinamis vector
#include <sstream>           // untuk manipulasi string stream
#include <ctime>             // untuk menentukan tanggal dan waktu
#include <nlohmann/json.hpp> // untuk menggunakan json dan bekerja dengan file json
using json = nlohmann::json;
using namespace std;

// pendeklarasian struct user untuk menyimpan data-data user
struct User
{
    string username;
    string password;
    string role;
    vector<struct BukuDiPinjam> buku_dipinjam;
    double denda;
};
// pendeklarasian BukuDiPinjam untuk menyimpan data buku yang dipinjam user
struct BukuDiPinjam
{
    int id;
    string judul;
    string author;
    string tanggal_pinjam;
    string tanggal_kembali;
};
// pendeklarasian struct buku untuk menyimpan data-data buku
struct Buku
{
    int id;
    string judul;
    string author;
    string isbn;
    int tahun;
    string path;
    int stock_baca;
    int stock_jual;
    double harga;
};

struct Transaksi
{
    string tanggal;
    string kategori;
    double jumlah;
    string deskripsi;
};

// pendefinisian fungsi/prosedur
void bersihkanLayar();
json dataUserJson();
void updateDataUser(vector<User> &data_user);
json dataBukuJson();
void updateDataBuku(vector<Buku> &data_buku);
void updateDataKeuangan(vector<Transaksi> &data_keuangan);
User login(vector<User> &data_user);
void signUp(vector<User> &data_user);
void tampilkanBuku(vector<Buku> &data_buku);
void tambahBuku(vector<Buku> &data_buku);
void ubahDataBuku(vector<Buku> &data_buku);
void hapusDataBuku(vector<Buku> &data_buku);
void tampilkanDataUser(vector<User> &data_user);
void tampilkanBukuUser(vector<Buku> &data_buku);
void laporanKeuangan(vector<Transaksi> data_transaksi);
void bacaBuku(vector<Buku> &data_buku);
void pinjamBuku(vector<Buku> &data_buku, vector<User> &data_user, string username, vector<BukuDiPinjam> &buku_dipinjam);
time_t stringKeTimeT(const string &tanggal);
void tampilkanBukuDipinjam(vector<User> &data_user, string username);
void kembalikanBuku(vector<Buku> &data_buku, vector<User> &data_user, string username);
void simpanTransaksiCSV(const string &tanggal, const string &kategori, double jumlah, const string &deskripsi);
void beliBuku(vector<Buku> &data_buku, string username);
void berandaAdmin(vector<Buku> &data_buku, vector<User> &data_user, vector<Transaksi> &data_transaksi);
void berandaUser(vector<Buku> &data_buku, vector<User> &data_user, string username, vector<BukuDiPinjam> &buku_dipinjam);

int main()
{
    // fungsi main berisi alur program utama
    int opsi;
    string role;
    vector<User> data_user;
    vector<Buku> data_buku;
    vector<BukuDiPinjam> buku_dipinjam;
    vector<Transaksi> data_transaksi;
    User user;
    bersihkanLayar();
    dataUserJson();
    updateDataUser(data_user);
    dataBukuJson();
    updateDataBuku(data_buku);
    updateDataKeuangan(data_transaksi);

    cout << "========== Program E-Library ==========\n";
    cout << "1. Login\n2. Signup\nPilih opsi (1/2) : ";
    do
    {
        cin >> opsi;
    } while (opsi > 2 || opsi < 1);
    if (opsi == 1)
    {
        cin.ignore();
        user = login(data_user);
    }
    else
    {
        cin.ignore();
        signUp(data_user);
        updateDataUser(data_user);
        cout << endl;
        user = login(data_user);
    }
    cout << user.username << " " << user.role << endl;
    if (user.role == "admin")
    {
        berandaAdmin(data_buku, data_user, data_transaksi);
    }
    else
    {
        berandaUser(data_buku, data_user, user.username, buku_dipinjam);
    }
    return 0;
}

void bersihkanLayar()
{
// prosedur bersihkanLayar digunakan untuk membersihkan tampilan layar terminal
    #ifdef _WIN32
        system("cls");
    #elif __linux__ || _APPLE_
        system("clear");
    #else
        return;
    #endif
}

json dataUserJson()
{
    // fungsi dataUserJson berfungsi untuk mengambil data-data yang ada di dalam file user.json ke json user
    ifstream file_user("data_user.json");
    if (!file_user.is_open())
    {
        cout << "Error: Tidak dapat memuat file data_user.json\n";
        return nullptr;
    }
    json user;
    file_user >> user;
    file_user.close();
    return user;
}

void updateDataUser(vector<User> &data_user){
    // prosedur updateDataUser berfungsi untuk memperbarui data user dari file json ke dalam vector data_user
    json user = dataUserJson();
    data_user.clear();
    for (auto &usrJson : user["User"])
    {
        User usr;
        usr.username = usrJson["username"];
        usr.password = usrJson["password"];
        usr.role = usrJson["role"];
        usr.denda = usrJson["denda"];

        usr.buku_dipinjam.clear();

        for (auto &bukuJson : usrJson["buku_dipinjam"])
        {
            BukuDiPinjam buku;

            buku.id = bukuJson["id"];
            buku.judul = bukuJson["judul"];
            buku.author = bukuJson["author"];
            buku.tanggal_pinjam = bukuJson["tanggal_pinjam"];
            buku.tanggal_kembali = bukuJson["tanggal_kembali"];

            usr.buku_dipinjam.push_back(buku);
        }

        data_user.push_back(usr);
    }
}

void simpanDataUser(const vector<User> &data_user)
{
    // prosedur simpanDataUser digunakan untuk menyimpan data user ke dalam file data_user.json
    json user;
    for (const auto &u : data_user)
    {
        json dipinjam = json::array();
        for (const auto &b : u.buku_dipinjam)
        {
            dipinjam.push_back({{"id", b.id},
                                {"judul", b.judul},
                                {"author", b.author},
                                {"tanggal_pinjam", b.tanggal_pinjam},
                                {"tanggal_kembali", b.tanggal_kembali}});
        }
        user["User"].push_back({{"username", u.username},
                                {"password", u.password},
                                {"role", u.role},
                                {"buku_dipinjam", dipinjam},
                                {"denda", u.denda}});
    }
    ofstream write_data_user("data_user.json");
    if (!write_data_user)
    {
        cout << "Error: Tidak dapat memuat file data_user.json\n";
        return;
    }
    write_data_user << user.dump(4);
}

json dataBukuJson()
{
    // fungsi dataBukuJson berfungsi untuk mengambil data-data yang ada di dalam file data_buku.json ke json data
    ifstream file_data_buku("data_buku.json");
    if (!file_data_buku.is_open())
    {
        cout << "Error: Tidak dapat memuat file data_buku.json\n";
        return nullptr;
    }
    json data;
    file_data_buku >> data;
    file_data_buku.close();
    return data;
}

void updateDataBuku(vector<Buku> &data_buku)
{
    // prosedur updateDataBuku berfungsi untuk memperbarui data buku dari file json ke dalam vector data_buku
    json data = dataBukuJson();
    data_buku.clear();
    for (auto &item : data["Buku"])
    {
        Buku buku;
        buku.id = item["id"];
        buku.judul = item["judul"];
        buku.author = item["author"];
        buku.isbn = item["isbn"];
        buku.tahun = item["tahun"];
        buku.path = item["path"];
        buku.stock_baca = item["stock_baca"];
        buku.stock_jual = item["stock_jual"];
        buku.harga = item["harga"];
        data_buku.push_back(buku);
    }
}

void simpanDataBuku(const vector<Buku> &data_buku)
{
    // prosedur simpanDataBuku digunakan untuk menyimpan data buku ke dalam file data_buku.json
    json data;
    for (const auto &buku : data_buku)
    {
        data["Buku"].push_back({{"id", buku.id},
                                {"judul", buku.judul},
                                {"author", buku.author},
                                {"isbn", buku.isbn},
                                {"tahun", buku.tahun},
                                {"path", buku.path},
                                {"stock_baca", buku.stock_baca},
                                {"stock_jual", buku.stock_jual},
                                {"harga", buku.harga}});
    }
    ofstream write_data_buku("data_buku.json");
    if (!write_data_buku)
    {
        cout << "Error: Tidak dapat memuat file data_buku.json\n";
        return;
    }
    write_data_buku << data.dump(4);
}

void updateDataKeuangan(vector<Transaksi> &data_keuangan)
{
    data_keuangan.clear();
    ifstream file("laporan_keuangan.csv");

    if (!file.is_open())
    {
        cout << "Error : Tidak dapat membuka file\n";
        return;
    }

    string baris;
    getline(file, baris); // lewati baris header

    while (getline(file, baris))
    {
        stringstream ss(baris);
        Transaksi t;
        string temp;

        getline(ss, t.tanggal, ',');
        getline(ss, t.kategori, ',');
        getline(ss, temp, ',');
        t.jumlah = stod(temp);
        getline(ss, t.deskripsi, ',');

        data_keuangan.push_back(t);
    }

    file.close();
    return;
}

User login(vector<User> &data_user)
{
    // fungsi logi untuk memvalidasi login user
    bersihkanLayar();
    updateDataUser(data_user);
    while (true)
    {
        string input_username, input_passwd;
        cout << "Masukan username anda : ";
        getline(cin, input_username);   
        cout << "Masukan password anda : ";
        getline(cin, input_passwd);
        for (auto user : data_user)
        {
            if ((input_username == user.username) && (input_passwd == user.password))
            {
                cout << "login berhasil" << endl;
                return {user.username, user.password, user.role};
            }
        }
        cout << "username atau password salah, silahkan coba lagi" << endl;
    }
}

void signUp(vector<User> &data_user)
{
    // prosedur singUp untuk menambah data user baru
    bersihkanLayar();
    string input_username, input_passwd;
    // menginput username serta mengecek apakah username sudah digunakan
    while (true)
    {
        bool valid = false;
        cout << "Buat username anda : ";
        getline(cin, input_username);
        for (auto user : data_user)
        {
            if (user.username == input_username)
            {
                cout << "Username telah digunakan !, tolong gunakan username lain\n";
                valid = true;
                break;
            }
        }
        if (!valid)
        {
            break;
        }
    }
    // menginput password dengan minimal 8 karakter
    while (input_passwd.length() < 8)
    {
        cout << "Buat password anda (minimal 8 karakter) : ";
        getline(cin, input_passwd);
    }
    // memperbarui data user di file
    User user_baru;
    user_baru.username = input_username;
    user_baru.password = input_passwd;
    user_baru.role = "user";
    user_baru.buku_dipinjam = {};
    user_baru.denda = 0.0;

    data_user.push_back(user_baru);
    simpanDataUser(data_user);
    cout << "user berhasil dibuat !\n";
    cout << "(Tekan Enter untuk login)";
    cin .ignore();
    updateDataUser(data_user);
}

void berandaAdmin(vector<Buku> &data_buku, vector<User> &data_user, vector<Transaksi> &data_transaksi)
{
    bersihkanLayar();
    int opsi;
    do
    {
        cout << "====================== Beranda Admin ======================\n";
        cout << "1. Tambah buku\t2. Lihat data buku\t3. Edit data buku\n4. Hapus buku\t5. Lihat data user\t6. Laporan keuangan\n7. Exit\n";
        cout << "===========================================================\n";
        cout << "Pilih opsi (1-7) : ";
        cin >> opsi;
        while (opsi > 7 || opsi < 1)
        {
            cout << "Opsi tidak valid, silahkan pilih opsi (1-7) : ";
            cin >> opsi;
        }
        switch (opsi)
        {
        case 1:
            tambahBuku(data_buku);
            break;
        case 2:
            tampilkanBuku(data_buku);
            break;
        case 3:
            ubahDataBuku(data_buku);
            break;
        case 4:
            hapusDataBuku(data_buku);
            break;
        case 5:
            tampilkanDataUser(data_user);
            break;
        case 6:
            laporanKeuangan(data_transaksi);
            break;
        case 7:
            cout << "Keluar dari program....\n";
            break;
        default:
            cout << "Opsi tidak valid";
        }
    } while (opsi != 7);
}

void berandaUser(vector<Buku> &data_buku, vector<User> &data_user, string username, vector<BukuDiPinjam> &buku_dipinjam)
{
    bersihkanLayar();
    int opsi;
    do
    {
        cout << "================= Program E-Library ==================\n";
        cout << "  Akun : " << username << endl;
        tampilkanBukuUser(data_buku);
        cout << "======================================================\n";
        cout << "1. Baca buku\t2. Pinjam buku\t3. Kembalikan buku\n4. Beli Buku\t5. Exit\n";
        cout << "======================================================\n";
        do
        {
            cout << "Pilih opsi (1-5) : ";
            cin >> opsi;
        } while (opsi < 1 || opsi > 6);

        switch (opsi)
        {
        case 1:
            bacaBuku(data_buku);
            break;
        case 2:
            pinjamBuku(data_buku, data_user, username, buku_dipinjam);
            break;
        case 3:
            kembalikanBuku(data_buku, data_user, username);
            break;
        case 4:
            beliBuku(data_buku, username);
            break;
        case 5:
            cout << "Keluar dari program....\n";
            break;
        default:
            cout << "Opsi tidak valid";
        }
    } while (opsi != 5);
}

void tampilkanBuku(vector<Buku> &data_buku)
{
    // prosedur untuk menampilkan buku_buku untuk admin
    bersihkanLayar();
    updateDataBuku(data_buku);

    cout << "================== DAFTAR BUKU ==========================\n";
    for (const auto &buku : data_buku)
    {
        cout << "[" << buku.id << "] " << buku.judul << '\n';
        cout << "    Author        : " << buku.author << '\n'
             << "    Tahun Terbit  : " << buku.tahun << '\n'
             << "    Harga         : Rp " << buku.harga << '\n'
             << "    Stock Baca    : " << buku.stock_baca << '\n'
             << "    Stock Jual    : " << buku.stock_jual << '\n'
             << "    ISBN          : " << buku.isbn << '\n'
             << "    Path File     : " << buku.path << '\n';
        cout << "-----------------------------------------------------------\n";
    }
}

void tambahBuku(vector<Buku> &data_buku)
{
    // prosedur untuk menambahkan buku baru
    Buku buku_baru;
    char konfirmasi ;
    bool create_data = true;
    json data = dataBukuJson();
    bool tambah_data = true;
    int jumlah_buku = data_buku.size();

    bersihkanLayar();
    while (create_data)
    {
        while (tambah_data)
        {
            cin.ignore();
            buku_baru.id = ++jumlah_buku;
            cout << "Input judul buku : ";
            getline(cin, buku_baru.judul);
            cout << "Input nama author buku : ";
            getline(cin, buku_baru.author);
            cout << "Input kode ISBN buku : ";
            getline(cin, buku_baru.isbn);
            cout << "Input tahun rilis buku : ";
            cin >> buku_baru.tahun;
            cin.ignore();
            cout << "Input path lokasi buku : ";
            getline(cin, buku_baru.path);
            cout << "Input jumlah stock baca : ";
            cin >> buku_baru.stock_baca;
            cout << "Input jumlah stock jual : ";
            cin >> buku_baru.stock_jual;
            cout << "Input harga buku : Rp ";
            cin >> buku_baru.harga;
            do
            {
                cout << "Apakah data sudah benar ? (y/n) : ";
                cin >> konfirmasi;
            } while (tolower(konfirmasi) != 'y' && tolower(konfirmasi) != 'n');
            if (tolower(konfirmasi) == 'y')
            {
                tambah_data = false;
            }
        }
        data["Buku"].push_back({{"id", buku_baru.id},
                                {"judul", buku_baru.judul},
                                {"author", buku_baru.author},
                                {"isbn", buku_baru.isbn},
                                {"tahun", buku_baru.tahun},
                                {"path", buku_baru.path},
                                {"stock_baca", buku_baru.stock_baca},
                                {"stock_jual", buku_baru.stock_jual},
                                {"harga", buku_baru.harga}});
        ofstream write_data_buku("data_buku.json");
        if (!write_data_buku)
        {
            cout << "Gagal memuat file !";
            return;
        }
        write_data_buku << data.dump(4);
        write_data_buku.close();
        cout << "Berhasil menambahkan data !. \n";
        updateDataBuku(data_buku);
        while(true){
            cout << "Ingin lanjut menambahkan data ? (y/n) : ";
            cin >> konfirmasi;
            if(tolower(konfirmasi) == 'y'){
                tambah_data = true;
                break;
            }else{
                create_data = false;
                break;
            }
        }
    }
    bersihkanLayar();
}

void ubahDataBuku(vector<Buku> &data_buku)
{
    // prosedur untuk merubah data buku
    json data = dataBukuJson();
    int id;
    char konfirmasi;
    bool update_data = true;
    bool ubah_data = true;
    string temp;

    bersihkanLayar();
    while (update_data)
    {
        tampilkanBuku(data_buku);
        do
        {
            cout << "Pilih data buku yang ingin diubah sesuai nomor : ";
            cin >> id;
        } while (id < 1 || id > data_buku.size());
        while (ubah_data)
        {
            for (auto &buku : data["Buku"])
            {
                if (buku["id"] == id)
                {
                    cout << "Mengubah data buku nomor = " << id << endl;
                    cin.ignore();
                    cout << "Input judul buku : ";
                    getline(cin, temp);
                    buku["judul"] = temp;
                    cout << "Input nama author buku : ";
                    getline(cin, temp);
                    buku["author"] = temp;
                    cout << "Input kode ISBN buku : ";
                    getline(cin, temp);
                    buku["isbn"];
                    cout << "Input tahun rilis buku : ";
                    cin >> buku["tahun"];
                    cout << "Input path lokasi buku : ";
                    getline(cin, temp);
                    buku["path"] = temp;
                    cout << "Input jumlah stock baca : ";
                    cin >> buku["stock_baca"];
                    cout << "Input jumlah stock jual : ";
                    cin >> buku["stock_jual"];
                    cout << "Input harga buku : Rp ";
                    cin >> buku["harga"];
                    break;
                }
            }
            do
            {
                cout << "Apakah data sudah benar ? (y/n) : ";
                cin >> konfirmasi;
            } while (tolower(konfirmasi) != 'y' && tolower(konfirmasi) != 'n');
            if (tolower(konfirmasi) == 'y')
            {
                ubah_data = false;
            }
        }

        ofstream write_data_buku("data_buku.json");
        if (!write_data_buku)
        {
            cout << "Gagal memuat file !";
            return;
        }
        write_data_buku << data.dump(4);
        write_data_buku.close();
        cout << "Berhasil merubah data !. \n";
        while (true)
        {
            cout << "Ingin lanjut merubah data ? (y/n) : ";
            cin >> konfirmasi;
            if (tolower(konfirmasi) == 'y')
            {
                break;
            }
            else
            {
                update_data = false;
                break;
            }
        }
    }
    bersihkanLayar();
    
    updateDataBuku(data_buku);
}

void hapusDataBuku(vector<Buku> &data_buku)
{
    // prosedur untuk menghapus data buku
    json data = dataBukuJson();
    int id;
    int id_baru = 1;
    char konfirmasi;

    bersihkanLayar();
    while (true)
    {
        tampilkanBuku(data_buku);
        do
        {
            cout << "Pilih data buku yang ingin duhapus sesuai nomor : ";
            cin >> id;
        } while (id < 1 || id > data_buku.size());
        do
        {
            cout << "Apakah anda yakin ingin menghapus buku ini ? (y/n) : ";
            cin >> konfirmasi;
        } while (tolower(konfirmasi) != 'y' && tolower(konfirmasi) != 'n');
        if (tolower(konfirmasi) == 'y')
        {
            for (auto i = data["Buku"].begin(); i != data["Buku"].end(); ++i)
            {
                if ((*i)["id"] == id)
                {
                    data["Buku"].erase(i);
                    break;
                }
            }
            for (auto &buku : data["Buku"])
            {
                buku["id"] = id_baru++;
            }
            ofstream write_data_buku("data_buku.json");
            if (!write_data_buku)
            {
                cout << "Gagal memuat file !";
                return;
            }
            write_data_buku << data.dump(4);
            write_data_buku.close();
            cout << "Berhasil menghapus data !. \n";
        }
        do
        {
            cout << "Ingin lanjut menghapus data ? (y/n) : ";
            cin >> konfirmasi;
        } while (tolower(konfirmasi) != 'y' && tolower(konfirmasi) != 'n');
        if (tolower(konfirmasi) == 'n')
        {
            break;
        }
    }
    bersihkanLayar();
    updateDataBuku(data_buku);
}

void tampilkanDataUser(vector<User> &data_user)
{
    // prosedur untuk menampilkan data user
    bersihkanLayar();
    updateDataUser(data_user);
    cout << "======================== Data User ========================\n";
    cout << "-----------------------------------------------------------\n";
    for (const auto &usr : data_user)
    {
        cout << "Username : " << usr.username << '\n';
        cout << "Password : " << usr.password << '\n';
        cout << "Buku Dipinjam:\n";
        if (usr.buku_dipinjam.empty())
        {
            cout << "  - Tidak ada buku yang dipinjam\n";
        }
        else
        {
            for (const auto &buku : usr.buku_dipinjam)
            {
                cout << "  ID Buku          : " << buku.id << '\n';
                cout << "  Judul            : " << buku.judul << '\n';
                cout << "  Author           : " << buku.author << '\n';
                cout << "  Tanggal Pinjam   : " << buku.tanggal_pinjam << '\n';
                cout << "  Tanggal Kembali  : " << buku.tanggal_kembali << '\n';
            }
        }
        cout << "Denda : Rp " << usr.denda << '\n';
        cout << "-----------------------------------------------------------\n";
    }
    cout << "===========================================================\n";
}

void laporanKeuangan(vector<Transaksi> data_transaksi)
{
    bersihkanLayar();
    updateDataKeuangan(data_transaksi);
    cout << "=====================================================================\n";
    cout << "  Tanggal  | Kategori  | Jumlah\t| deskripsi\n";
    cout << "=====================================================================\n";
    for (const auto &t : data_transaksi)
    {
        cout << t.tanggal << " | "
             << t.kategori << " | "
             << t.jumlah << "\t| "
             << t.deskripsi << '\n';
    }
    cout << "=====================================================================\n";
}

void tampilkanBukuUser(vector<Buku> &data_buku)
{
    // prosedur untuk menampilkan buku pada user biasa
    updateDataBuku(data_buku);
    cout << "==================== Daftar Buku =====================\n";
    cout << "------------------------------------------------------\n";
    for (auto &buku : data_buku)
    {
        cout << "[" << buku.id << "] " << buku.judul << endl;
        cout << "    Author : " << buku.author << endl;
        cout << "    Stock  : " << buku.stock_baca << endl;
        cout << "------------------------------------------------------\n";
    }
}

void bacaBuku(vector<Buku> &data_buku)
{
    // prosedur untuk membuka buku
    int id_buku;
    string path;

    do
    {
        cout << "Input ID buku yang ingin dibaca: ";
        cin >> id_buku;
    } while (id_buku < 1 || id_buku > data_buku.size());
    for (auto &buku : data_buku)
    {
        if (buku.id == id_buku)
        {
            path = buku.path;
            break;
        }
    }
    #ifdef _WIN32
        string command = "start \"\" \"" + path + "\"";
    #elif __linux__
        string command = "xdg-open ." + path;
    #elif __APPLE__
        string command = "open \"" + path + "\"";
    #else
        std::cerr << "OS tidak didukung\n";
        return;
    #endif

    int result = system(command.c_str());
    if (result != 0)
    {
        std::cerr << "Gagal membuka file PDF\n";
    }
    bersihkanLayar();
}

void pinjamBuku(vector<Buku> &data_buku, vector<User> &data_user, string username, vector<BukuDiPinjam> &buku_dipinjam)
{
    // prosedur untuk meminjam buku
    int id;
    json data;
    json user;
    BukuDiPinjam buku_pinjam;
    time_t sekarang = time(nullptr);
    tm sekarang_tm = *localtime(&sekarang);
    time_t kembali = sekarang + (7 * 24 * 60 * 60);
    tm kembali_tm = *localtime(&kembali);
    char tgl_sekarang[11];
    char tgl_kembali[11];
    strftime(tgl_sekarang, sizeof(tgl_sekarang), "%d-%m-%Y", &sekarang_tm);
    strftime(tgl_kembali, sizeof(tgl_kembali), "%d-%m-%Y", &kembali_tm);

    bersihkanLayar();
    tampilkanBukuUser(data_buku);

    for (auto &usr : data_user)
    {
        if (usr.username == username)
        {
            if (usr.buku_dipinjam.size() >= 2)
            {
                cout << "maaf, hanya dapat meminjam maksimal 2 buku" << endl;
                cout << "(tekan Enter untuk lanjut)";
                cin.ignore();
                cin.ignore();
                bersihkanLayar();
                return;
            }
        }
    }
    cout << "Aturan meminjam buku : \n1. Maksimal meminjam 2 buku\n2. Waktu peminjaman selama 7 hari\n3. Denda Rp 2000/hari jika terlambat mengembalikan buku\n";
    do
    {
        cout << "Input id buku yang ingin dipinjam : ";
        cin >> id;
    } while (id < 1 || id > data_buku.size());
    for (auto &buku : data_buku)
    {
        if (buku.id == id)
        {
            if (buku.stock_baca > 0)
            {
                buku.stock_baca -= 1;
                buku_pinjam.id = buku.id;
                buku_pinjam.judul = buku.judul;
                buku_pinjam.author = buku.author;
                buku_pinjam.tanggal_pinjam = tgl_sekarang;
                buku_pinjam.tanggal_kembali = tgl_kembali;
                simpanDataBuku(data_buku);
            }
            else
            {
                cout << "maaf stock buku habis" << endl;
            }
            break;
        }
    }
    for (auto &usr : data_user)
    {
        if (usr.username == username)
        {
            usr.buku_dipinjam.push_back(buku_pinjam);
            simpanDataUser(data_user);
            cout << "berhasil meminjam buku" << endl;
            cout << "(tekan Enter untuk lanjut)";
            cin.ignore();
            cin.ignore();
        }
    }
    updateDataUser(data_user);
    updateDataBuku(data_buku);
}

time_t stringKeTimeT(const string &tanggal)
{
    // fungsi untuk mengkonversi string tanggal ke time_t
    tm waktu = {};

    waktu.tm_mday = std::stoi(tanggal.substr(0, 2));       
    waktu.tm_mon = std::stoi(tanggal.substr(3, 2)) - 1;    
    waktu.tm_year = std::stoi(tanggal.substr(6, 4)) - 1900; 

    waktu.tm_hour = 0;
    waktu.tm_min = 0;
    waktu.tm_sec = 0;
    waktu.tm_isdst = -1;

    return mktime(&waktu);
}

void tampilkanBukuDipinjam(vector<User> &data_user, string username)
{
    updateDataUser(data_user);
    cout << "================ Buku yang Dipinjam ==================\n";
    cout << "------------------------------------------------------\n";
    for (auto &usr : data_user)
    {
        if (usr.username == username)
        {
            for (auto &buku : usr.buku_dipinjam)
            {
                cout << "[" << buku.id << "] " << buku.judul << '\n';
                cout << "    Author : " << buku.author << '\n';
                cout << "    Tanggal pinjam  : " << buku.tanggal_pinjam << '\n';
                cout << "    Tanggal kembali  : " << buku.tanggal_kembali << '\n';
                cout << "------------------------------------------------------\n";
            }
            cout << "======================================================\n";
        }
    }
}

void kembalikanBuku(vector<Buku> &data_buku, vector<User> &data_user, string username)
{
    // proedur untuk mengembalikan buku
    int i = 0;
    int id_buku;
    bool kembalikan_buku = true;
    char konfirmasi;
    double denda, pembayaran, kembalian;
    time_t sekarang = time(nullptr);
    tm sekarang_tm = *localtime(&sekarang);
    char tgl_sekarang[11];
    strftime(tgl_sekarang, sizeof(tgl_sekarang), "%d-%m-%Y", &sekarang_tm);
    time_t pinjam;
    time_t kembali;

    bersihkanLayar();
    tampilkanBukuDipinjam(data_user, username);
    cout << "Tanggal hari ini : " << tgl_sekarang << endl;

    while (kembalikan_buku)
    {
        cout << "Input Id buku yang ingin dikembalikan : ";
        cin >> id_buku;
        for (auto &usr : data_user)
        {
            if (usr.username == username)
            {
                for (auto &buku : usr.buku_dipinjam)
                {
                    if (buku.id == id_buku)
                    {
                        pinjam = stringKeTimeT(buku.tanggal_pinjam);
                        kembali = stringKeTimeT(buku.tanggal_kembali);
                        double selisih_detik = difftime(sekarang, kembali);
                        int selisih_hari = selisih_detik / (60 * 60 * 24);
                        if (selisih_hari <= 0)
                        {
                            for (auto &buku : data_buku)
                            {
                                if (buku.id == id_buku)
                                {
                                    buku.stock_baca += 1;
                                    simpanDataBuku(data_buku);
                                    break;
                                }
                            }
                            for (auto &usr : data_user)
                            {
                                if (usr.username == username)
                                {
                                    for(auto& buku : usr.buku_dipinjam){
                                        if(buku.id == id_buku){
                                            usr.buku_dipinjam.erase(usr.buku_dipinjam.begin()+i);
                                            break;
                                        }
                                        i++;
                                    }
                                    usr.denda = 0.0;
                                    simpanDataUser(data_user);
                                    break;
                                }
                            }
                            cout << "berhasil mengembalikan buku" << endl;
                            kembalikan_buku = false;
                        }
                        else
                        {
                            cout << "Terlambat " << selisih_hari << " hari\n";
                            denda = selisih_hari * 2000;
                            cout << "Denda: Rp " << denda << endl;
                            cout << "Apakah anda ingin membayar denda sekarang ? (y/n) : ";
                            do{
                                cin >> konfirmasi;
                            } while (konfirmasi != 'y' && konfirmasi != 'n');
                            if (konfirmasi == 'y')
                            {
                                while (true)
                                {
                                    cout << "Masukan jumlah pembayaran : Rp ";
                                    cin >> pembayaran;
                                    if (pembayaran >= denda)
                                    {
                                        kembalian = pembayaran - denda;
                                        cout << "Pembayaran berhasil. Kembalian anda : Rp " << kembalian << endl;
                                        simpanTransaksiCSV(tgl_sekarang, "Pemasukan", denda, "denda keterlambatan buku oleh " + username);
                                        for (auto &buku : data_buku)
                                        {
                                            if (buku.id == id_buku)
                                            {
                                                buku.stock_baca += 1;
                                                simpanDataBuku(data_buku);
                                            }
                                        }
                                        for (auto &usr : data_user)
                                        {
                                            if (usr.username == username)
                                            {
                                                for(auto& buku : usr.buku_dipinjam){
                                                    if(buku.id == id_buku){
                                                        usr.buku_dipinjam.erase(usr.buku_dipinjam.begin()+i);
                                                        break;
                                                    }
                                                    i++;
                                                }
                                                usr.denda = 0.0;
                                                simpanDataUser(data_user);
                                                break;
                                            }
                                        }
                                        cout << "berhasil mengembalikan buku\n";
                                        kembalikan_buku = false;
                                        break;
                                    }
                                    else
                                    {
                                        cout << "Jumlah pembayaran kurang. Silahkan coba lagi. \n";
                                    }
                                }
                            }
                            else
                            {
                                cout << "Silahkan bayar denda anda saat mengembalikan buku berikutnya." << endl;
                                kembalikan_buku = false;
                                for (auto &usr : data_user)
                                {
                                    if (usr.username == username)
                                    {
                                        usr.denda += denda;
                                        simpanDataUser(data_user);
                                        break;
                                    }
                                }
                            }
                            
                        }
                    }
                }
            }
        }
    }
    cout << "(tekan Enter untuk lanjut)";
    cin.ignore();
    cin.ignore();
    updateDataUser(data_user);
    updateDataBuku(data_buku);
}

void beliBuku(vector<Buku> &data_buku, string username)
{
    int id_buku;
    double pembayaran, kembalian, pemasukan;
    string judul;
    json data;
    time_t sekarang = time(nullptr);
    tm sekarang_tm = *localtime(&sekarang);
    char tgl_sekarang[11];
    strftime(tgl_sekarang, sizeof(tgl_sekarang), "%d-%m-%Y", &sekarang_tm);

    bersihkanLayar();
    updateDataBuku(data_buku);

    cout << "==================== Daftar Buku =====================\n";
    cout << "------------------------------------------------------\n";
    for (auto &buku : data_buku)
    {
        cout << "[" << buku.id << "] " << buku.judul << endl;
        cout << "    Author : " << buku.author << endl;
        cout << "    Stock  : " << buku.stock_jual << endl;
        cout << "    Harga  : " << buku.harga << endl;
        cout << "------------------------------------------------------\n";
    }
    cout << "===========================================================\n";
    cout << "Input id buku yang ingin dibeli : ";
    do
    {
        cin >> id_buku;
    } while (id_buku < 1 && id_buku > data_buku.size());
    for (auto &buku : data_buku)
    {
        if (buku.id == id_buku)
        {
            if (buku.stock_jual > 0)
            {
                cout << "Harga buku = Rp " << buku.harga << endl;
                while (true)
                {
                    cout << "Masukan jumlah pembayaran : Rp ";
                    cin >> pembayaran;
                    if (pembayaran >= buku.harga)
                    {
                        kembalian = pembayaran - buku.harga;
                        pemasukan = buku.harga;
                        judul = buku.judul;
                        cout << "Kembalian anda = Rp " << kembalian << endl;
                        buku.stock_jual -= 1;
                        simpanDataBuku(data_buku);
                        break;
                    }
                    else
                    {
                        cout << "Jumlah pembayaran kuran. Silahkan coba kembali.\n";
                    }
                }
            }
            else
            {
                cout << "Maaf stock buku kosong\n";
                return;
            }
            break;
        }
    }
   
    cout << "Berhasil membeli buku !\n";
    cout << "(tekan Enter untuk lanjut)";
    cin.ignore();
    cin.ignore();
    bersihkanLayar();
    updateDataBuku(data_buku);
    simpanTransaksiCSV(tgl_sekarang, "Penjualan", pemasukan, "Buku " + judul + " dibeli oleh " + username);
}

void simpanTransaksiCSV(const string &tanggal, const string &kategori, double jumlah, const string &deskripsi)
{
    ofstream file("laporan_keuangan.csv", ios::app);
    if (!file.is_open())
    {
        cout << "Gagal membuka file CSV\n";
        return;
    }
    file << endl
         << tanggal << ","
         << kategori << ","
         << jumlah << ","
         << deskripsi << ",";
    file.close();
}
