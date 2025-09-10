// 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Struktur data untuk setiap tugas
typedef struct tugas {
    char nama[50];                  // Nama tugas
    char deskripsi[200];            // Deskripsi tugas
    time_t deadline;                // Waktu deadline dalam epoch
    int intervalpengulangan;        // Interval pengulangan (hari), 0 jika tidak berulang
    int status;                     // 0 = pending, 1 = done
    struct tugas *next;             // Pointer ke node berikutnya
} tugas;

const char *FILE_TUGAS = "./todo.txt"; // Nama file untuk penyimpanan

// Prototipe fungsi
void memuat_tugas(tugas **head);
void simpan_tugas(tugas *head);
void tambah_tugas(tugas **head);
void lihat_tugas(tugas *head);
void edit_tugas(tugas *head);
void tandai_tugas(tugas *head);
void hapus_tugas(tugas **head);
void tampilkan_tugas(tugas *head);
tugas *buat_simpul(const char *nama, const char *deskripsi, time_t deadline, int intervalpengulangan);
void masukkan_simpul(tugas **head, tugas *simpul_baru);
time_t input_tanggalwaktu();
void hapus_semua(tugas *head);

int main() {
    tugas *head = NULL;              
    int pilihan;

    memuat_tugas(&head);              // Muat data dari file jika ada

    do {
        // Menu utama
        printf("\n=== PERSONAL TO-DO LIST ===\n");
        printf("1. Tambah tugas\n");
        printf("2. Lihat tugas\n");
        printf("3. Edit tugas\n");
        printf("4. Tandai selesai/belum\n");
        printf("5. Hapus tugas\n");
        printf("0. Keluar\n");
        printf("Pilih: ");
        scanf("%d", &pilihan);
        getchar();             

        switch (pilihan) {
            case 1: tambah_tugas(&head); break;
            case 2: lihat_tugas(head); break;
            case 3: edit_tugas(head); break;
            case 4: tandai_tugas(head); break;
            case 5: hapus_tugas(&head); break;
            case 0:
                simpan_tugas(head);   
                hapus_semua(head);     
                printf("Keluar...\n");
                break;
            default:
                printf("Pilihan tidak valid!\n");
        }
    } while (pilihan != 0);

    return 0;
}

// Fungsi membaca file dan membangun linked list
void memuat_tugas(tugas **head) {
    FILE *filepointer = fopen(FILE_TUGAS, "r"); 
    if (!filepointer) return;                  

    char line[512];
    while (fgets(line, sizeof(line), filepointer)) {
        char nama[50], deskripsi[200];
        long deadline;
        int interval, status;
        // format: nama|deskripsi|deadline|interval|status\n
        if (sscanf(line, "%49[^|]|%199[^|]|%ld|%d|%d", nama, deskripsi, &deadline, &interval, &status) == 5) {
            tugas *node = buat_simpul(nama, deskripsi, (time_t)deadline, interval);
            node->status = status;      
            masukkan_simpul(head, node);
        }
    }
    fclose(filepointer);                        
}

// Fungsi menyimpan semua tugas ke file
void simpan_tugas(tugas *head) {
    FILE *filepointer = fopen(FILE_TUGAS, "w"); 
    tugas *bantu = head;
    while (bantu) {
        //format: nama|deskripsi|deadline|interval|status\n
        fprintf(filepointer, "%s|%s|%ld|%d|%d\n",
                bantu->nama,
                bantu->deskripsi,
                (long)bantu->deadline,
                bantu->intervalpengulangan,
                bantu->status);
        bantu = bantu->next;
    }
    fclose(filepointer);                        
}

// Fungsi membuat node baru dengan alokasi memori
tugas *buat_simpul(const char *nama, const char *deskripsi, time_t deadline, int intervalpengulangan) {
    tugas *node = (tugas *)malloc(sizeof(tugas)); 
    strcpy(node->nama, nama);           
    strcpy(node->deskripsi, deskripsi);           
    node->deadline = deadline;          
    node->intervalpengulangan = intervalpengulangan; 
    node->status = 0;                   
    node->next = NULL;                  
    return node;                        
}

// Fungsi memasukkan node baru secara terurut berdasarkan deadline
void masukkan_simpul(tugas **head, tugas *simpul_baru) {
    if (!*head || simpul_baru->deadline < (*head)->deadline) {
        // Jika linkedlist kosong atau deadline lebih awal dari head
        simpul_baru->next = *head;         
        *head = simpul_baru;               
    } else {
        tugas *bantu = *head;
        // Cari posisi tepat: sebelum node dengan deadline >= simpul_baru
        while (bantu->next && bantu->next->deadline < simpul_baru->deadline){
            bantu = bantu->next;    
        }
        simpul_baru->next = bantu->next;     
        bantu->next = simpul_baru;
    }
}

// Fungsi input tanggal & waktu dari user dan kembalikan epoch
time_t input_tanggalwaktu() {
    char databaru[20];
    int year, month, day, hour, minute;
    struct tm t = {0};                // Inisialisasi struct tm
    printf("Masukkan deadline (YYYY-MM-DD HH:MM): ");
    fgets(databaru, sizeof(databaru), stdin);
    sscanf(databaru, "%d-%d-%d %d:%d", &year, &month, &day, &hour, &minute);
    t.tm_year = year - 1900;          // Tahun sejak 1900
    t.tm_mon = month - 1;             // Bulan 0-11
    t.tm_mday = day;                  // Hari 1-31
    t.tm_hour = hour;                 // Jam 0-23
    t.tm_min = minute;                // Menit 0-59
    t.tm_sec = 0;
    return mktime(&t);                // Konversi ke epoch
}

// Fungsi tambah tugas baru
void tambah_tugas(tugas **head) {
    char nama[50];
    char deskripsi[200];
    int interval;
    printf("Masukkan nama tugas: ");
    fgets(nama, sizeof(nama), stdin);
    nama[strcspn(nama, "\n")] = 0;  
    printf("Masukkan deskripsi tugas: ");
    fgets(deskripsi, sizeof(deskripsi), stdin);
    deskripsi[strcspn(deskripsi, "\n")] = 0;  
    time_t dl = input_tanggalwaktu();    
    printf("Interval pengulangan (hari, 0 jika hanya sekali): ");
    scanf("%d", &interval);
    getchar();                        
    
    tugas *node = buat_simpul(nama, deskripsi, dl, interval);
    masukkan_simpul(head, node);
    simpan_tugas(*head);                
    printf("Tugas berhasil ditambahkan!\n");
}

// Fungsi menampilkan daftar tugas dengan filter
void lihat_tugas(tugas *head) {
    if (!head) {
        printf("Tidak ada tugas.\n");
        return;
    }
    int hari;
    printf("Lihat tugas dalam rentang hari (0 = semua): ");
    scanf("%d", &hari);
    getchar();
    time_t now = time(NULL);
    tugas *bantu = head;
    int index = 1;
    while (bantu) {
        double selisihwkt = difftime(bantu->deadline, now) / (60 * 60 * 24);
        if (hari == 0 || selisihwkt <= hari) {
            // Tampilkan tugas sesuai filter
            char tanggal[20];
            strftime(tanggal, sizeof(tanggal), "%Y-%m-%d %H:%M", localtime(&bantu->deadline));
            printf("%d. [%s] %s - %s (Deadline: %s, Interval: %d hari)\n",
                   index,
                   bantu->status ? "Selesai" : "Belum Selesai",
                   bantu->nama,
                   bantu->deskripsi,
                   tanggal,
                   bantu->intervalpengulangan);
            index++;
        }
        bantu = bantu->next;
    }
}

// Fungsi menampilkan list tugas dengan indeks
void tampilkan_tugas(tugas *head) {
    tugas *bantu = head;
    int index = 1;
    char tanggal[20];
    while (bantu) {
        strftime(tanggal, sizeof(tanggal), "%Y-%m-%d %H:%M", localtime(&bantu->deadline));
        printf("%d. [%s] %s (Deadline: %s)\n", index,
               bantu->status ? "Selesai" : "Belum Selesai",
               bantu->nama,
               tanggal);
        bantu = bantu->next;
        index++;
    }
}

// Fungsi edit nama, deskripsi, deadline, interval tugas
void edit_tugas(tugas *head) {
    if (!head) {
        printf("Tidak ada tugas untuk diedit.\n");
        return;
    }
    tampilkan_tugas(head);            
    int nomor;
    printf("Pilih nomor tugas untuk edit: ");
    scanf("%d", &nomor);
    getchar();
    tugas *bantu = head;
    int index = 1;
    while (bantu && index < nomor) {
        bantu = bantu->next;
        index++;
    }
    if (!bantu) {
        printf("Nomor tidak valid.\n");
        return;
    }
    // Input data baru
    char databaru[200];
    printf("Nama baru (%s): ", bantu->nama);
    fgets(databaru, sizeof(databaru), stdin);
    databaru[strcspn(databaru, "\n")] = 0;
    if (strlen(databaru)) strcpy(bantu->nama, databaru);
    printf("Deskripsi baru (%s): ", bantu->deskripsi);
    fgets(databaru, sizeof(databaru), stdin);
    databaru[strcspn(databaru, "\n")] = 0;
    if (strlen(databaru)) strcpy(bantu->deskripsi, databaru);
    printf("Deadline baru (kosong untuk tidak berubah): ");
    fgets(databaru, sizeof(databaru), stdin);
    if (strstr(databaru, "-")) {
        time_t deadlinebaru;
        int y, mo, d, h, mi;
        if (sscanf(databaru, "%d-%d-%d %d:%d", &y, &mo, &d, &h, &mi) == 5) {
            struct tm tm2 = {0};
            tm2.tm_year = y - 1900;
            tm2.tm_mon = mo - 1;
            tm2.tm_mday = d;
            tm2.tm_hour = h;
            tm2.tm_min = mi;
            deadlinebaru = mktime(&tm2);
            bantu->deadline = deadlinebaru;
        }
    }
    printf("Interval baru (%d): ", bantu->intervalpengulangan);
    fgets(databaru, sizeof(databaru), stdin);
    int intervalBaru;
    if (sscanf(databaru, "%d", &intervalBaru) == 1) bantu->intervalpengulangan = intervalBaru;
    simpan_tugas(head);                
    printf("Tugas berhasil diupdate!\n");
}

// Fungsi menandai tugas selesai/belum
void tandai_tugas(tugas *head) {
    if (!head) {
        printf("Tidak ada tugas untuk ditandai.\n");
        return;
    }
    tampilkan_tugas(head);            
    int nomor;
    printf("Pilih nomor tugas untuk toggle status: ");
    scanf("%d", &nomor);
    getchar();
    tugas *bantu = head;
    int index = 1;
    while (bantu && index < nomor) {
        bantu = bantu->next;
        index++;
    }
    if (!bantu) {
        printf("Nomor tidak valid.\n");
        return;
    }
    bantu->status = !bantu->status;      
    simpan_tugas(head);                
    printf("Status tugas diubah menjadi [%s]!\n", bantu->status ? "Selesai" : "Belum Selesai");
}

// Fungsi menghapus tugas berdasarkan nomor
void hapus_tugas(tugas **head) {
    if (!*head) {
        printf("Tidak ada tugas untuk dihapus.\n");
        return;
    }
    tampilkan_tugas(*head);           
    int nomor;
    printf("Pilih nomor tugas untuk dihapus: ");
    scanf("%d", &nomor);
    getchar();
    tugas *bantu = *head, *prev = NULL;
    int index = 1;
    while (bantu && index < nomor) {
        prev = bantu;
        bantu = bantu->next;
        index++;
    }
    if (!bantu) {
        printf("Nomor tidak valid.\n");
        return;
    }
    if (prev) prev->next = bantu->next; 
    else *head = bantu->next;           
    free(bantu);                        
    simpan_tugas(*head);                
    printf("Tugas berhasil dihapus!\n");
}

// Fungsi membebaskan seluruh linked list
void hapus_semua(tugas *head) {
    tugas *bantu = head;
    while (bantu) {
        tugas *tmp = bantu;
        bantu = bantu->next;
        free(tmp);
    }
}
