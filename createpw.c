#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#define MAX_INTS 10
#define handle_error(msg)                               \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

pthread_mutex_t INTLIST_MUTEX = PTHREAD_MUTEX_INITIALIZER;

int intlist [MAX_INTS];

typedef struct thread_info_t
{
    pthread_t thread_id;
    int thread_num;
    int value;
} thread_info_t;


int get_limit(int *limit)
{

    fprintf(stdout, "how many words would you like int your pw 1-10? ");
    char line[128];

    if (fgets(line,sizeof(line),stdin))
    {
        fprintf(stdout,"line: %s",line);
        sscanf(line,"%d",limit);
    }
    return ((*limit > 0) && (*limit <11));
}


static void*
populate_intlist(void * arg)
{
    thread_info_t * tinfo=arg;
    
    int value=tinfo->value;
    pthread_mutex_lock(&INTLIST_MUTEX);
    if (value > 66666){
        fprintf(stderr, "ERROR: invalid value: the digit you entered is > 66666\n");
        intlist[tinfo->thread_num]=-1;
    }
    else if (value < 0)
    {
        fprintf(stderr, "ERROR: invalid value: the digit you entered is < 0\n");
        intlist[tinfo->thread_num]=-2;
    }
    else
    {
        intlist[tinfo->thread_num]=value;
    }
    pthread_mutex_unlock(&INTLIST_MUTEX);
    return intlist;
}

int grep_for_word(int num)
{
    assert(num > 0);
    char *search;
    int MAXCOL = 64;
    char needle[MAXCOL];
    /* snprintf (buff, sizeof(buf), "%d",n); // print int 'n' into the char[] buffer */
    snprintf(needle,sizeof(needle),"%d",num);
    fprintf(stdout, "num: %d\n",num);
    FILE * read_fd;
    read_fd = fopen("./wordfiles/eff_large_wordlist.txt", "r");
    char line[128];
    char testWord[128];
    int  testNum;
    
    while ((search = fgets(line,sizeof(line),read_fd)) != NULL)
    {
        //fprintf(stdout,"line: %s\n",line);
        //sscanf(line,"%*[^(]%[^)]",str);
        sscanf(line,"%d %s",&testNum,testWord);
        if (testNum==num)
        {
            fprintf(stdout, "SUCCESS: found: %d - with word: \"%s\"\n",testNum,testWord);
            break;
        }
        
        if (line ==NULL || testWord==NULL )
        {
            fprintf(stderr, "ERROR: NOT FOUND:  %d - with word %s\n",testNum,testWord);
            break;
        }
    }
    return 0;
}


int main(int argc, char**argv)
{

    /** ask for the number of password words we need */

    int ret;
    int limit;
    ret = get_limit(&limit);
    if (!ret)
    {
        fprintf(stderr,"ERROR: limit out of range 1-10\n");
        exit(-1);
    }

    /* next we spawn enough threads for each one to handle one 5 int input + 
       the grep for the word
    */
    int tnum;
    thread_info_t * thread_list;
    thread_list = calloc(limit, sizeof(struct thread_info_t));
    if (thread_list == NULL)
        handle_error("calloc");

    /* Create one thread for each command-line argument */

    for (tnum = 0; tnum < limit; tnum++) {
        char line[128];
        int value=-9;
        fprintf(stdout, "enter a five digit integer: ");
        if (fgets(line,sizeof(line),stdin))
        {
            sscanf(line,"%d",&value);
            if (value > 66666){
                fprintf(stderr, "ERROR: invalid value: the digit you entered is > 66666\n");
            }
            else if (value < 0)
            {
                fprintf(stderr, "ERROR: invalid value: the digit you entered is < 0\n");
            }
        }

        thread_list[tnum].thread_num = tnum ;
        thread_list[tnum].value=value;
        ret = pthread_create(&thread_list[tnum].thread_id, NULL,
                           &populate_intlist, &thread_list[tnum]);
        if (ret != 0)
            handle_error("pthread_create");
    }

    /** make everything else into -1  */
    for (; tnum < MAX_INTS;tnum++)
    {
        intlist[tnum]=-1;
    }
    

    /* next we ask for a number of symbols */

    /* and finally we put it all together in 1 password where each word is
       separated by a symbol , with each odd word (with a zero index start) 
       becomes all capatalized
    */
    

    int i;
    for (i =0;i<MAX_INTS;i++){
        // fprintf(stdout, "intlist[%d]=%d\n",i,intlist[i]);
        if (intlist[i] >0){
            grep_for_word(intlist[i]);
        }
    }
    return 0;
}
