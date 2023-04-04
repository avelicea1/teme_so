#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int listDir(const char *path, int recursive, const char *name_start_with, const char *permissions, int *k)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf;
    dir = opendir(path);
    long number = 0;
    int numberOctal = 0;
    if (dir == NULL)
    {
        closedir(dir);
        perror("Could not open dir");
        return -1;
    }
    for (int i = 0; i < 9 && strlen(permissions) > 0; i++)
    {
        if (permissions[i] == 'r' || permissions[i] == 'w' || permissions[i] == 'x')
        {
            number = number + 1;
            number = number * 10;
        }
        else
        {
            if (permissions[i] == '-')
            {
                number = number * 10;
            }
        }
    }
    number = number / 10;

    int remainder = 0;
    int decimal = 0;
    for (int i = 1; number != 0; i = i * 2, number = number / 10)
    {
        remainder = number % 10;
        decimal = decimal + remainder * i;
    }
    for (int i = 1; decimal != 0; i = i * 10)
    {
        numberOctal = numberOctal + (decimal % 8) * i;
        decimal = decimal / 8;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        if (recursive == 1)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
                if (lstat(fullPath, &statbuf) == 0)
                {
                    int mode = statbuf.st_mode & 0X777;
                    int modeOctal = 0;
                    for (int i = 1; mode != 0; i = i * 10)
                    {
                        modeOctal = modeOctal + (mode % 8) * i;
                        mode = mode / 8;
                    }
                    if (strncmp(name_start_with, entry->d_name, strlen(name_start_with)) == 0 && (modeOctal == numberOctal || numberOctal == 0))
                    {
                        (*k)++;
                        printf("%s\n", fullPath);
                    }
                    if (S_ISDIR(statbuf.st_mode))
                    {
                        listDir(fullPath, recursive, name_start_with, permissions, k);
                    }
                }
            }
        }
        else
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {

                snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
                if (lstat(fullPath, &statbuf) == 0)
                {
                    int mode = statbuf.st_mode & 0777;
                    int modeOctal = 0;
                    for (int i = 1; mode != 0; i = i * 10)
                    {
                        modeOctal = modeOctal + (mode % 8) * i;
                        mode = mode / 8;
                    }
                    if (strncmp(name_start_with, entry->d_name, strlen(name_start_with)) == 0 && (modeOctal == numberOctal || numberOctal == 0))
                    {
                        (*k)++;
                        printf("%s\n", fullPath);
                    }
                }
            }
        }
    }
    closedir(dir);
    return 0;
}
struct sectionHeader
{
    char name[14];
    unsigned int sect_type;
    int sect_offset;
    int sect_size;
} sectionHeader;
int parse(const char *path)
{
    int fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        printf("ERROR \n invalid path");
        return -1;
    }
    lseek(fd, -1, SEEK_END);
    char magic;
    if (read(fd, &magic, 1) == 1)
    {
        if (magic != 'l')
        {
            printf("ERROR \nwrong magic");
            return -1;
        }
    }
    if (magic == 'l')
    {

        lseek(fd, -3, SEEK_END);
        int headerSize = 0;
        if (read(fd, &headerSize, 2) != 2)
        {
            perror("could not read size");
        }
        lseek(fd, -headerSize, SEEK_END);
        unsigned char c;
        int version = 0;
        int nrSections = 0;
        if (read(fd, &c, 1) != 1)
        {
            perror("could not read version!");
        }
        version = c;
        if (read(fd, &nrSections, 1) != 1)
        {
            perror("could not read no_of_sections!");
        }
        struct sectionHeader **sectionsHeaders = (struct sectionHeader **)malloc(sizeof(struct sectionHeader) * nrSections);
        for (int i = 1; i <= nrSections; i++)
        {
            sectionsHeaders[i] = (struct sectionHeader *)malloc(sizeof(struct sectionHeader));
        }
        if (version >= 38 && version <= 126)
        {
            if (nrSections >= 3 && nrSections <= 19)
            {
                for (int i = 1; i <= nrSections; i++)
                {
                    char name[14];
                    if (read(fd, &name, 14) != 14)
                    {
                        printf("could not read name");
                    }
                    strcpy(sectionsHeaders[i]->name, name);
                    sectionsHeaders[i]->name[14] = 0;
                    unsigned int sect_type = 0;
                    if (read(fd, &sect_type, 2) != 2)
                    {
                        printf("could not read sect_type");
                    }
                    sectionsHeaders[i]->sect_type = 0;
                    sectionsHeaders[i]->sect_type = sect_type;
                    if (sect_type == 60 || sect_type == 75 || sect_type == 91)
                    {
                        int sect_offset = 0;
                        if (read(fd, &sect_offset, 4) != 4)
                        {
                            printf("could not read sect_offset");
                        }
                        sectionsHeaders[i]->sect_offset = 0;
                        sectionsHeaders[i]->sect_offset = sect_offset;
                        // printf("%d\n",sect_offset);
                        int sect_size = 0;
                        if (read(fd, &sect_size, 4) != 4)
                        {
                            printf("could not read sect_size");
                        }
                        sectionsHeaders[i]->sect_size = 0;
                        sectionsHeaders[i]->sect_size = sect_size;
                    }
                    else
                    {
                        printf("ERROR \nwrong sect_types");
                        for (int i = 1; i <= nrSections; i++)
                        {
                            free(sectionsHeaders[i]);
                        }
                        free(sectionsHeaders);
                        return -1;
                    }
                }
            }
            else
            {
                printf("ERROR \nwrong sect_nr");
                for (int i = 1; i <= nrSections; i++)
                {
                    free(sectionsHeaders[i]);
                }
                free(sectionsHeaders);
                return -1;
            }
        }
        else
        {
            printf("ERROR \nwrong version");
            for (int i = 1; i <= nrSections; i++)
            {
                free(sectionsHeaders[i]);
            }
            free(sectionsHeaders);
            return -1;
        }
        printf("SUCCESS\n");
        printf("version=%d\n", version);
        printf("nr_sections=%d\n", nrSections);
        for (int i = 1; i <= nrSections; i++)
        {
            printf("section%d: ", i);
            printf("%s ", sectionsHeaders[i]->name);
            printf("%d ", sectionsHeaders[i]->sect_type);
            printf("%d ", sectionsHeaders[i]->sect_size);
            printf("\n");
        }
        for (int i = 1; i <= nrSections; i++)
        {
            free(sectionsHeaders[i]);
        }
        free(sectionsHeaders);
    }
    close(fd);
    return 0;
}

int findAll(const char *path, int *m)
{

    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf;
    dir = opendir(path);
    if (dir == NULL)
    {
        closedir(dir);
        printf("Could not open dir");
        return -1;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
            if (lstat(fullPath, &statbuf) == 0)
            {
                if (*m == 0)
                {
                    printf("SUCCESS\n");
                    (*m) = 1;
                }
                // printf("%s\n",fullPath);
                int k = 0;
                int n = 0;
                if (S_ISREG(statbuf.st_mode))
                {
                    int fd = open(fullPath, O_RDONLY);
                    if (fd == -1)
                    {
                        printf("ERROR \n invalid path");
                        return -1;
                    }
                    lseek(fd, -1, SEEK_END);
                    char magic;
                    read(fd, &magic, 1);
                    if (magic == 'l')
                    {

                        // printf("%d\n",version);
                        lseek(fd, -3, SEEK_END);
                        int headerSize = 0;
                        if (read(fd, &headerSize, 2) != 2)
                        {
                            perror("could not read size");
                        }
                        lseek(fd, -headerSize, SEEK_END);
                        int version = 0;
                        if (read(fd, &version, 1) != 1)
                        {
                            perror("could not read version");
                        }

                        int nrSections = 0;
                        if (read(fd, &nrSections, 1) != 1)
                        {
                            perror("could not read no_of_sections!");
                        }
                        // printf("sect = %d\n",nrSections);
                        for (int i = 1; i <= nrSections; i++)
                        {
                            char name[14];
                            if (read(fd, &name, 14) != 14)
                            {
                                printf("could not read name");
                            }
                            unsigned int sect_type = 0;
                            if (read(fd, &sect_type, 2) != 2)
                            {
                                printf("could not read sect_type");
                            }
                            int sect_offset = 0;
                            if (read(fd, &sect_offset, 4) != 4)
                            {
                                printf("could not read sect_offset");
                            }
                            int sect_size = 0;
                            if (read(fd, &sect_size, 4) != 4)
                            {
                                printf("could not read sect_size");
                            }
                            // printf("sect type = %d \n",sect_type);
                            if (nrSections >= 3 && nrSections <= 19)
                            {
                                if (version >= 38 && version <= 126)
                                {
                                    if (sect_type == 60 || sect_type == 75 || sect_type == 91)
                                    {
                                        if (sect_type == 60)
                                        {
                                            k++;
                                        }
                                        n++;
                                    }
                                }
                            }
                        }
                        // printf("n= %d\n",n);
                        // printf("k=%d\n",k);
                        if (k >= 3 && n == nrSections && nrSections >= 3)
                        {
                            printf("%s\n", fullPath);
                        }
                        close(fd);
                    }
                }
                if (S_ISDIR(statbuf.st_mode))
                {
                    findAll(fullPath, m);
                }
            }
        }
    }
    closedir(dir);
    return 0;
}

int extract(const char *path, int section, int line)
{

    int fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        printf("ERROR \ninvalid file");
        return -1;
    }
    char magic;
    lseek(fd, -1, SEEK_END);
    if (read(fd, &magic, 1) != 1)
    {
        printf("erorr read magic");
    }
    if (magic == 'l')
    {
        lseek(fd, -3, SEEK_END);
        int headerSize = 0;
        if (read(fd, &headerSize, 2) != 2)
        {
            perror("could not read size");
        }
        lseek(fd, -headerSize, SEEK_END);
        unsigned char c;
        int version = 0;
        int nrSections = 0;
        if (read(fd, &c, 1) != 1)
        {
            perror("could not read version!");
        }
        version = c;
        if (read(fd, &nrSections, 1) != 1)
        {
            perror("could not read no_of_sections!");
        }
        struct sectionHeader **sectionsHeaders = (struct sectionHeader **)malloc(sizeof(struct sectionHeader) * nrSections);
        for (int i = 1; i <= nrSections; i++)
        {
            sectionsHeaders[i] = (struct sectionHeader *)malloc(sizeof(struct sectionHeader));
        }
        for (int i = 1; i <= nrSections; i++)
        {
            char name[14];
            if (read(fd, &name, 14) != 14)
            {
                printf("could not read name");
            }
            strcpy(sectionsHeaders[i]->name, name);
            sectionsHeaders[i]->name[14] = 0;
            unsigned int sect_type = 0;
            if (read(fd, &sect_type, 2) != 2)
            {
                printf("could not read sect_type");
            }
            sectionsHeaders[i]->sect_type = 0;
            sectionsHeaders[i]->sect_type = sect_type;
            int sect_offset = 0;
            if (read(fd, &sect_offset, 4) != 4)
            {
                printf("could not read sect_offset");
            }
            sectionsHeaders[i]->sect_offset = 0;
            sectionsHeaders[i]->sect_offset = sect_offset;
            int sect_size = 0;

            if (read(fd, &sect_size, 4) != 4)
            {
                printf("could not read sect_size");
            }
            sectionsHeaders[i]->sect_size = 0;
            sectionsHeaders[i]->sect_size = sect_size;
            off_t seek_cur = lseek(fd, 0, SEEK_CUR);

            if (sect_type == 60 || sect_type == 75 || sect_type == 91)
            {
                if (version >= 38 && version <= 126)
                {
                    if (sectionsHeaders[i]->sect_type == 60 || sectionsHeaders[i]->sect_type == 75 || sectionsHeaders[i]->sect_type == 91)
                    {
                        
                        //int lungime_linie = 0;
                        if (i == section)
                        {
                            lseek(fd, sectionsHeaders[i]->sect_offset, SEEK_SET);
                            char *buff = (char *)calloc((sectionsHeaders[i]->sect_size + 1), sizeof(char));
                            char *linieString = (char *)malloc((sectionsHeaders[i]->sect_size + 1) * sizeof(char));
                            // printf("linia 1:");
                            //  printf("size = %d\n",sectionsHeaders[i]->sect_size);
                            int j=0;
                            int line_nr = 1;
                            int poz_inceput = 0;
                            int n=0;
                            //int copie_poz_inceput =0;
                            while (j <= sectionsHeaders[i]->sect_size)
                            {
                                // printf("BUN");
                                if (read(fd, &c, 1) == 1)
                                {
                                    buff[j] = c;
                                    // printf("poz inceput %d\n",poz_inceput);
                                    // printf("lungime linie %d\n",j-poz_inceput-1);
                                    //printf("%c", buff[j]);
                                    
                                    if (j>0 && (int)buff[j] == 0x0A && (int)buff[j - 1] == 0x0D )
                                    {
                                        //printf("%d %d \n",j,sectionsHeaders[i]->sect_size);
                                        if (line_nr == line)
                                        {
                                            
                                            //printf("%d ",j);
                                            //printf("%d ",poz_inceput);
                                            //printf("%d ",j-poz_inceput);
                                            //printf("%d",line_nr);
                                            printf("SUCCESS\n");
                                            //for(int m = poz_inceput;m<j-poz_inceput-1;m++){
                                                //printf("%c",buff[poz_inceput+m]);
                                              //  linieString[m]=buff[poz_inceput+m];
                                            //}
                                            //printf("%c",linieString[j-poz_inceput]);
                                            //linieString[j-poz_inceput]=0;
                                            //char *copielinieString = (char *)malloc((sectionsHeaders[i]->sect_size + 1) * sizeof(char));
                                            //for(int n = 0; n<j-poz_inceput-1;j++){
                                                //printf("%c",linieString[0]);
                                                //copielinieString[n] = linieString [j-poz_inceput-n-1];
                                            //}
                                            //copielinieString[j-poz_inceput]=0;
                                            n=0;
                                            for(int m = j-1;m>=poz_inceput;m--){
                                                linieString[n++]=buff[m];
                                            }
                                            linieString[n]=0;
                                            printf("%s",linieString);
                                            for (int s = 1; s <= nrSections; s++)
                                            {
                                                free(sectionsHeaders[s]);
                                            }
                                            free(sectionsHeaders);
                                            free(buff);
                                            free(linieString);
                                            return 0;
                                        }
                                        // strrev(linieString);
                                        //printf("%d",line_nr);
                                        line_nr++;
                                        //copie_poz_inceput = poz_inceput;
                                        poz_inceput = j + 1;
                                        // printf("\nlinie noua\n");
                                        // printf("\n");
                                        // printf("linia %d:",line_nr);
                                    }else{
                                        
                                        if(line_nr == line){
                                            
                                            linieString[n++]=buff[j];
                                        }
                                    }
                                }
                                j++;
                            }
                            if(line_nr == line){
                                printf("SUCCESS\n");
                                char *copielinieString = (char *)malloc((sectionsHeaders[i]->sect_size + 1) * sizeof(char));
                                int k=0;
                                //printf("%d",strlen(linieString));
                                printf("%c",linieString[strlen(linieString)-1]);
                                for(int i=strlen(linieString)-2;i>=0;i--){
                                    copielinieString[k++]=linieString[i];
                                }
                                copielinieString[k]=0;
                                printf("%s",copielinieString);
                            }
                            
                            //printf("%c",buff[sectionsHeaders[i]->sect_size]);
                            //printf("lungime_linie = %d \n", lungime_linie);
                            // printf("%ld", seek_cur);
                             //printf("nr linii= %d\n", line_nr);
                            if (line_nr < line)
                            {
                                printf("ERROR\ninvalid line");
                                for (int s = 1; s <= nrSections; s++)
                                {
                                    free(sectionsHeaders[s]);
                                }
                                free(sectionsHeaders);
                                free(buff);
                                free(linieString);
                                return -1;
                            }
                            // printf("%d", nrSections);
                            if (section > nrSections)
                            {
                                printf("ERROR\ninvalid section");
                                for (int s = 1; s <= nrSections; s++)
                                {
                                    free(sectionsHeaders[s]);
                                }
                                free(sectionsHeaders);
                                free(buff);
                                free(linieString);
                                return -1;
                            }
                        }
                    }
                }
            }
            else
            {
                for (int s = 1; s <= nrSections; s++)
                {
                    free(sectionsHeaders[s]);
                }
                free(sectionsHeaders);
                return -1;
            }

            lseek(0, seek_cur, SEEK_SET);
        }

        for (int s = 1; s <= nrSections; s++)
        {
            free(sectionsHeaders[s]);
        }
        free(sectionsHeaders);
    }
    close(fd);
    return 0;
}
int main(int argc, char **argv)
{
    char name_start_with[512] = "";
    char permissions[10] = "";
    char path[512] = "";
    int k = 0;
    int r = 0, l = 0, p = 0, f = 0, e = 0;
    int line = 0, section = 0;
    if (argc >= 2)
    {
        if (strcmp(argv[1], "variant") == 0)
        {
            printf("21546\n");
        }
        else
        {
            for (int i = 1; i < argc; i++)
            {
                if (strstr(argv[i], "name_starts_with=") != NULL)
                {
                    strcpy(name_start_with, argv[i] + 17);
                }
                if (strstr(argv[i], "path=") != NULL)
                {
                    strcpy(path, argv[i] + 5);
                }
                if (strstr(argv[i], "permissions=") != NULL)
                {
                    strcpy(permissions, argv[i] + 12);
                }
                if (strcmp(argv[i], "recursive") == 0)
                {
                    r = 1;
                }
                if (strcmp(argv[i], "list") == 0)
                {
                    l = 1;
                }
                if (strcmp(argv[i], "parse") == 0)
                {
                    p = 1;
                }
                if (strcmp(argv[i], "findall") == 0)
                {
                    f = 1;
                }
                if (strstr(argv[i], "section=") != NULL)
                {
                    section = atoi(argv[i] + 8);
                }
                if (strstr(argv[i], "line=") != NULL)
                {
                    line = atoi(argv[i] + 5);
                }
                if (strcmp(argv[i], "extract") == 0)
                {
                    e = 1;
                }
            }
            if (l != 0)
            {
                if (strcmp(path, "") != 0)
                {
                    DIR *dir = opendir(path);
                    if (dir == NULL)
                    {
                        closedir(dir);
                        printf("ERROR \ninvalid directory path");
                    }
                    else
                    {
                        printf("SUCCESS \n");

                        listDir(path, r, name_start_with, permissions, &k);
                        closedir(dir);
                    }
                }
                else
                {
                    printf("ERROR \npath is NULL");
                }
            }
            if (p != 0)
            {
                if (strcmp(path, "") != 0)
                {
                    parse(path);
                }
                else
                {
                    printf("ERROR \npath is NULL");
                }
            }
            if (f != 0)
            {
                if (strcmp(path, "") != 0)
                {
                    int m = 0;
                    findAll(path, &m);
                }
                else
                {
                    printf("ERROR \ninvalid directory path");
                }
            }
            if (e != 0)
            {
                if (strcmp(path, "") != 0)
                {
                    // int k=0;
                    extract(path, section, line);
                }
                else
                {
                    printf("ERROR \ninvalid path");
                }
            }
        }
    }

    return 0;
}