#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/wait.h>
#define SEGMENT 1024 //approximate target size of small file

static  const  char * dirPath = "/home/vincent/Documents";
char key[100] = "9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";

char kode1[10] = "encv1_";
char kode2[10] = "encv2_";

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

void encrypt1(char* str) { //encrypt encv1_
	if(strcmp(str, ".") == 0) return;
    if(strcmp(str, "..") == 0)return;
    printf("enkrip nih saiki\n");
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

void decrypt1(char * str){ //decrypt encv1_
	if(strcmp(str, ".") == 0) return;
    if(strcmp(str, "..") == 0)return;
	if(strstr(str, "/") == NULL)return;
    printf("DECRYPT ENC : %s\n",str);
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
	printf("HASIL DECRYPT ENC : %s\n",str);
}

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

static  int  xmp_getattr(const char *path, struct stat *stbuf){
	char * enc1 = strstr(path, kode1);
	printf("getattr path :%s : enc1 : %s\n",path,enc1);
	if(enc1 != NULL){
		printf("dekrip nih GETATTR\n");
		decrypt1(enc1);
    }
	char * enc2 = strstr(path, kode2);
	if(enc2 != NULL) {
        decrypt2(enc2);
    }
	char newPath[1000];
	int res;
	sprintf(newPath,"%s%s", dirPath, path);
	res = lstat(newPath, stbuf);
	if (res == -1)
		return -errno;
	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){ //baca directory
	char * enc1 = strstr(path, kode1);
    printf("\n--readdir path : %s : enc : %s\n", path, enc1);
	if(enc1 != NULL) {
		printf("dekrip nih READDIR\n"); 
        decrypt1(enc1);//buat ngebalikin biar bisa dibaca di document
    }
	char * enc2 = strstr(path, kode2);
	if(enc2 != NULL) {
        decrypt2(enc2);
    }

	char newPath[1000];
	if(strcmp(path,"/") == 0){
		path=dirPath;
		sprintf(newPath,"%s",path);
	} else sprintf(newPath, "%s%s",dirPath,path);

	int res = 0;
	struct dirent *dir;
	DIR *dp;
	(void) fi;
	(void) offset;
	dp = opendir(newPath);
	if (dp == NULL) return -errno;

	while ((dir = readdir(dp)) != NULL) { //buat loop yang ada di dalem directory
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = dir->d_ino;
		st.st_mode = dir->d_type << 12;
		if(enc1 != NULL){
			printf("d name : %s\n",dir->d_name);
			encrypt1(dir->d_name); //encrypt file/directory yng ada di dalam directory sekarang
        }
		if(enc2 != NULL){
			encrypt2(dir->d_name, enc2);
        }
		res = (filler(buf, dir->d_name, &st, 0));
		if(res!=0) break;
	}

	closedir(dp);
	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode){ //buat bikin directory baru

	char newPath[1000];
	if(strcmp(path,"/") == 0){
		path=dirPath;
		sprintf(newPath,"%s",path);
	}
	else sprintf(newPath, "%s%s",dirPath,path);

	int res = mkdir(newPath, mode);
    char str[100];
	sprintf(str, "MKDIR::%s", path);
	logSystem(str,1);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev){

	char newPath[1000];
	if(strcmp(path,"/") == 0){
		path = dirPath;
		sprintf(newPath,"%s",path);
	} else sprintf(newPath, "%s%s",dirPath,path);
	int res;

	if (S_ISREG(mode)) {
		res = open(newPath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(newPath, mode);
	else
		res = mknod(newPath, mode, rdev);
    char str[100];
	sprintf(str, "CREAT::%s", path);
	logSystem(str,1);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_unlink(const char *path) { //ngehapus file
	printf("unlink\n");
	char * enc1 = strstr(path, kode1);
	if(enc1 != NULL){
		printf("dekrip nih RMDIR\n");
        decrypt1(enc1); //buat ngebalikin biar bisa dibaca di document
    }
	char * enc2 = strstr(path, kode2);
	if(enc2 != NULL) {
        decrypt2(enc2);
    }

	char newPath[1000];
	if(strcmp(path,"/") == 0){
		path=dirPath;
		sprintf(newPath,"%s",path);
	} else sprintf(newPath, "%s%s",dirPath,path);
    char str[100];
	sprintf(str, "REMOVE::%s", path);
	logSystem(str,2);
	int res;
	res = unlink(newPath);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rmdir(const char *path) {//ngehapus directory
	printf("rmdir\n");
	char * enc1 = strstr(path, kode1);
	if(enc1 != NULL){
		printf("dekrip nih RMDIR\n");
        decrypt1(enc1); //buat ngebalikin biar bisa dibaca di document
    }
	char * enc2 = strstr(path, kode2);
	if(enc2 != NULL) {
        decrypt2(enc2);
    }

	char newPath[1000];
	sprintf(newPath, "%s%s",dirPath,path);
    char str[100];
	sprintf(str, "RMDIR::%s", path);
	logSystem(str,2);
	int res;
	res = rmdir(newPath);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rename(const char *from, const char *to) { //buat renme
	printf("\nRENAME!!!\n");
	char fileFrom[1000],fileTo[1000];
	sprintf(fileFrom,"%s%s",dirPath,from);
	sprintf(fileTo,"%s%s",dirPath,to);

    char str[100];
	sprintf(str, "RENAME::%s::%s", from, to);
	logSystem(str,1);
	int res;
	res = rename(fileFrom, fileTo);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi){ //open file
	char newPath[1000];
	sprintf(newPath, "%s%s",dirPath,path);
	int res;
	res = open(newPath, fi->flags);
	if (res == -1)
		return -errno;
	close(res);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){ //read file
	
	char newPath[1000];
	sprintf(newPath, "%s%s",dirPath,path);
	int fd;
	int res;

	(void) fi;
	fd = open(newPath, O_RDONLY);
	if (fd == -1)
		return -errno;
	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;
	close(fd);
	return res;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) { //write file
	
	char newPath[1000];
	sprintf(newPath, "%s%s", dirPath, path);
	
    int fd;
	int res;
	(void) fi;
	fd = open(newPath, O_WRONLY);
	if (fd == -1)
		return -errno;
    char str[100];
	sprintf(str, "WRITE::%s", path);
	logSystem(str,1);
	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;
	close(fd);
	return res;
}

static struct fuse_operations xmp_oper = {

	.getattr = xmp_getattr,
	.readdir = xmp_readdir,
	.read = xmp_read,
	.mkdir = xmp_mkdir,
	.mknod = xmp_mknod,
	.unlink = xmp_unlink,
	.rmdir = xmp_rmdir,
	.rename = xmp_rename,
	.open = xmp_open,
	.write = xmp_write,

};

int  main(int  argc, char *argv[]){
	umask(0);

	return fuse_main(argc, argv, &xmp_oper, NULL);
}
