# SoalShiftSISOP20_modul4_C06 
- Muhammad Afif Fadhlurrahman 05111840000093
- Vincentius Tanubrata 05111840000091  

Di suatu perusahaan, terdapat pekerja baru yang super jenius, ia bernama jasir. Jasir baru bekerja selama seminggu di perusahan itu, dalam waktu seminggu tersebut ia selalu terhantui oleh ketidak amanan dan ketidak efisienan file system yang digunakan perusahaan tersebut. Sehingga ia merancang sebuah file system yang sangat aman dan efisien. Pada segi keamanan, Jasir telah menemukan 2 buah metode enkripsi file. Pada mode enkripsi pertama, nama file-file pada direktori terenkripsi akan dienkripsi menggunakan metode substitusi. Sedangkan pada metode enkripsi yang kedua, file-file pada direktori terenkripsi akan di-split menjadi file-file kecil. Sehingga orang-orang yang tidak menggunakan filesystem rancangannya akan kebingungan saat mengakses direktori terenkripsi tersebut. Untuk segi efisiensi, dikarenakan pada perusahaan tersebut sering dilaksanakan sinkronisasi antara 2 direktori, maka jasir telah merumuskan sebuah metode agar filesystem-nya mampu mengsingkronkan kedua direktori tersebut secara otomatis. Agar integritas filesystem tersebut lebih terjamin, maka setiap command yang dilakukan akan dicatat kedalam sebuah file log.
(catatan: filesystem berfungsi normal layaknya linux pada umumnya)
(catatan: mount source (root) filesystem adalah direktori /home/[user]/Documents)

### Soal 1
Enkripsi versi 1:
- Jika sebuah direktori dibuat dengan awalan “encv1_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v1.
- Jika sebuah direktori di-rename dengan awalan “encv1_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v1.
- Apabila sebuah direktori terenkripsi di-rename menjadi tidak terenkripsi, maka isi adirektori tersebut akan terdekrip.
- Setiap pembuatan direktori terenkripsi baru (mkdir ataupun rename) akan tercatat ke sebuah database/log berupa file.
- Semua file yang berada dalam direktori ter enkripsi menggunakan caesar cipher dengan key.  
    ```9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO```  
  Misal kan ada file bernama “kelincilucu.jpg” dalam directory FOTO_PENTING, dan key yang dipakai adalah 10
  “encv1_rahasia/FOTO_PENTING/kelincilucu.jpg” => “encv1_rahasia/ULlL@u]AlZA(/g7D.|_.Da_a.jpg
  Note : Dalam penamaan file ‘/’ diabaikan, dan ekstensi tidak perlu di encrypt.
- Metode enkripsi pada suatu direktori juga berlaku kedalam direktori lainnya yang ada didalamnya.
### Pembahasan soal 1
eksripsi pada soal 1
``` c
void encrypt1(char* str) { //encrypt encv1_
	if(strcmp(str, ".") == 0) return;
    if(strcmp(str, "..") == 0)return;
    int str_length = strlen(str);
    for(int i = 0; i < str_length; i++) {
		if(str[i] == '/') continue;
		if(str[i]=='.') break;
        for(int j = 0; j < 87; j++) {
            if(str[i] == key[j]) {
                str[i] = key[(j+10) % 87];
                break;
            }
        }
    }
}
```
kita hanya melakukan enkripsi sampai sebelum `.`(ekstensi),maka kita break ketika bertemu `.` jika bertemu `/` akan di continue karena `/` tidak ikut dienkripsi, lalu kita enkripsi menggunakan key yang telah disediakan dengan menambah 10

dekripsi pada soal 1
```c
void decrypt1(char * str){ //decrypt encv1_
	if(strcmp(str, ".") == 0) return;
    if(strcmp(str, "..") == 0)return;
	if(strstr(str, "/") == NULL)return;
    int str_length = strlen(str),s=0;
	for(int i = str_length; i >= 0; i--){
		if(str[i]=='/')break;

		if(str[i]=='.'){//nyari titik terakhir
			str_length = i;
			break;
		}
	}
	for(int i = 0; i < str_length; i++){
		if(str[i]== '/'){
			s = i+1;
			break;
		}
	}
    for(int i = s; i < str_length; i++) {
		if(str[i] =='/'){
            continue;
        }
        for(int j = 0;j < 87; j++) {
            if(str[i] == key[j]) {
                str[i] = key[(j+77) % 87];
                break;
            }
        }
    }
}
```
untuk dekripsi, mirip dengan enkripsi hanya saja key nya ditambah 77 lalu di mod total keynya, agar kembali ke huruf awalnya
fungsi dekripsi dipanggil setiap awal get attribute, readir,dll. agar kita bisa membuka file yang tidak ter-enkrip di dokumen.

### Soal 2
Enkripsi versi 2:
- Jika sebuah direktori dibuat dengan awalan “encv2_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v2.
- Jika sebuah direktori di-rename dengan awalan “encv2_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v2.
- Apabila sebuah direktori terenkripsi di-rename menjadi tidak terenkripsi, maka isi direktori tersebut akan terdekrip.
- Setiap pembuatan direktori terenkripsi baru (mkdir ataupun rename) akan tercatat ke sebuah database/log berupa file.
- Pada enkripsi v2, file-file pada direktori asli akan menjadi bagian-bagian kecil sebesar 1024 bytes dan menjadi normal ketika diakses melalui filesystem rancangan jasir. Sebagai contoh, file File_Contoh.txt berukuran 5 kB pada direktori asli akan menjadi 5 file kecil yakni: File_Contoh.txt.000, File_Contoh.txt.001, File_Contoh.txt.002, File_Contoh.txt.003, dan File_Contoh.txt.004.
- Metode enkripsi pada suatu direktori juga berlaku kedalam direktori lain yang ada didalam direktori tersebut (rekursif).
### Pembahasan soal 2
enkripsi dan dekripsi nomor 2, membagi file berdasarkan size
```c
long file_size(char *name)
{
    FILE *fp = fopen(name, "rb"); //must be binary read to get bytes
    // printf("%s\n",name);
    long size=-1;
    if(fp)
    {
        fseek (fp, 0, SEEK_END);
        size = ftell(fp)+1;
        fclose(fp);
    }
    return size;
}

void encrypt2(char* enc, char * enc2){
    if(strcmp(enc, ".") == 0) return;
    if(strcmp(enc, "..") == 0)return;
    
    int segments=0, i, accum;

    char largeFileName[300];    //change to your path
    sprintf(largeFileName,"%s/%s/%s",dirPath,enc2,enc);
    printf("%s\n",largeFileName);

    char filename[260];//base name for small files.
    sprintf(filename,"%s.",largeFileName);
    //printf("%s\n",filename);

    char smallFileName[300];
    char line[1080];
    FILE *fp1, *fp2;
    long sizeFile = file_size(largeFileName);
    //printf("%ld\n",sizeFile);
    
    segments = sizeFile/SEGMENT + 1;//ensure end of file
    // printf("%d",segments);
    fp1 = fopen(largeFileName, "r");
    if(fp1)
    {
        char number[100];
        printf("disini\n");
        for(i=0;i<segments;i++)
        {
            accum = 0;
            if(i/10==0){
                sprintf(number,"00%d",i);
            }
            else if(i/100 == 0){
                sprintf(number,"0%d",i);
            }
            else if(i/1000 == 0){
                sprintf(number,"%d",i);
            }
            sprintf(smallFileName, "%s%s", filename, number);
            fp2 = fopen(smallFileName, "w");
            if(fp2)
            {
                while(fgets(line, 1080, fp1) && accum <= SEGMENT)
                {
                    accum += strlen(line);//track size of growing file
                    fputs(line, fp2);
                }
                fclose(fp2);
            }
        }
        fclose(fp1);
    }
    printf("keluar\n");
}

void decrypt2(char * enc){
    if(strcmp(enc, ".") == 0) return;
    if(strcmp(enc, "..") == 0)return;
	if(strstr(enc, "/") == NULL)return;
}
```
### Soal 3
Sinkronisasi direktori otomatis:

Tanpa mengurangi keumuman, misalkan suatu directory bernama dir akan tersinkronisasi dengan directory yang memiliki nama yang sama dengan awalan sync_ yaitu sync_dir. Persyaratan untuk sinkronisasi yaitu:
- Kedua directory memiliki parent directory yang sama.
- Kedua directory kosong atau memiliki isi yang sama. Dua directory dapat dikatakan memiliki isi yang sama jika memenuhi:
  - Nama dari setiap berkas di dalamnya sama.
  - Modified time dari setiap berkas di dalamnya tidak berselisih lebih dari 0.1 detik.
- Sinkronisasi dilakukan ke seluruh isi dari kedua directory tersebut, tidak hanya di satu child directory saja.
- Sinkronisasi mencakup pembuatan berkas/directory, penghapusan berkas/directory, dan pengubahan berkas/directory.

Jika persyaratan di atas terlanggar, maka kedua directory tersebut tidak akan tersinkronisasi lagi.
Implementasi dilarang menggunakan symbolic links dan thread.
### Pembahasan soal 3

### Soal 4
Log system:

- Sebuah berkas nantinya akan terbentuk bernama "fs.log" di direktori *home* pengguna (/home/[user]/fs.log) yang berguna menyimpan daftar perintah system call yang telah dijalankan.
- Agar nantinya pencatatan lebih rapi dan terstruktur, log akan dibagi menjadi beberapa level yaitu INFO dan WARNING.
- Untuk log level WARNING, merupakan pencatatan log untuk syscall rmdir dan unlink.
- Sisanya, akan dicatat dengan level INFO.
- Format untuk logging yaitu:  
  ```[LEVEL]::[yy][mm][dd]-[HH]:[MM]:[SS]::[CMD]::[DESC ...]```  
  
| Format | Description |
| --- | --- |
| LEVEL | Level logging |
| yy | Tahun dua digit |
| mm | Bulan dua digit |
| dd | Hari dua digit |
| HH | Jam dua digit |
| MM | Menit dua digit |
| SS | Detik dua digit |
| CMD | System call yang terpanggil |
| DESC | Deskripsi tambahan (bisa lebih dari satu, dipisahkan dengan ::) |

| Contoh format logging nantinya seperti: |
| --- |
| INFO::200419-18:29:28::MKDIR::/iz1 |
| INFO::200419-18:29:33::CREAT::/iz1/yena.jpg |
| INFO::200419-18:29:33::RENAME::/iz1/yena.jpg::/iz1/yena.jpeg |
### Pembahasan soal 4
fungsi log system warning dan info , disertai waktu dengan menggunakan `time_t`
```c
void logSystem(char* c, int type){
    FILE * logFile = fopen("/home/vincent/fs.log", "a");
	time_t currTime;
	struct tm * time_info;
	time ( &currTime );
	time_info = localtime (&currTime);
    int yr=time_info->tm_year,mon=time_info->tm_mon,day=time_info->tm_mday,hour=time_info->tm_hour,min=time_info->tm_min,sec=time_info->tm_sec;
    if(type==1){//info
        fprintf(logFile, "INFO::%d%d%d-%d:%d:%d::%s\n", yr, mon, day, hour, min, sec, c);
    }
    else if(type==2){ //warning
        fprintf(logFile, "WARNING::%d%d%d-%d:%d:%d::%s\n", yr, mon, day, hour, min, sec, c);
    }
    fclose(logFile);
}
```
