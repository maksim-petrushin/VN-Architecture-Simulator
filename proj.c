/*
Maksim Petrushin
VN Machine Simulator
08/22/23
*/

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <ctype.h>
#include "./constants.h"
#define COMPUTER_COMPONENTS 31
#define HALT_DELAY 1000
#define PROGRAM_COUNT 19

char curLines[7][20];
//instructions
char instructions[4076][75] = {
    //"5 5",   // IN <5>
    //"10 ",    // SKIPG
    //"8 20",   // JMP 20
    //"3 2",   // STORE 2
    //"5 5",   // IN <5>
    //"10 ",    // SKIPG
    //"8 24",   // JMP 25
    //"3 3",   // STORE 3
    //"1 3",   // LOAD 3
    //"2 4",   // ADD 4
    //"3 4",   // STORE 4
    //"1 2",   // LOAD 2
    //"4 1",   // SUBT 1 (value 1)
    //"3 2",   // STORE 2
    //"9",    // SKIPZ
    //"8 28",   // JMP 30 
    //"1 4",   // LOAD 3
    //"6 7",   // OUT <7>
    //"7"    // HALT
};
int speed = 1000;
int switched =0;
int waitTarget =0;
int gotTarget=0;
int getTarget =0;
int pcWait = 0;
int marToMem = 0;
int goingRight=0;
int goingLeft = 0;
int pcInc =0;
int animation=1;
int diference;
int halt = 0;
int accumTransfer  = 0;
int skip = 0;
int mdrToIr = 0;
int mdrToAccum = 0;
int pcPlusOne = 0;
int load =0;
int add =0;
int subt = 0;
int fetch = 0;
int mdrToMem = 0;
int arrived = 0;
int store = 0;
int addressToMar = 0;
int instructionLine = 0;
int accumReady = 0;
int inpt = 0;
int oupt = 0;
int currInstruction = -1;
int instructionActivating = 0;
int target_address = 0;
int game_is_running = false;
int last_frame_time;
int cursor_frame;
// 0 not pressed, 1 pressed
int mouseUpDown = 0;
int oper[4076];
int addres[4076];
char currInput[20] = "";
char accumText[20] = "0";
char mdrText[20] = "0";
char memText[4096][20];
char marText[20] = "0";
char pcText[20] = "20";
char opText[20] = "0";
char addressText[20] = "0";
char op[20] = "";
char address[20] = "";
int nOp = -1;
int nAddress = -1;
typedef struct computer_component
{
    int x;
    int y;
    int width;
    int height;
} computer_component;
int textW, textH;
int currMem = 0;
int stdOut = 0;
int numLines;

int initialize_window(SDL_Window **window, SDL_Renderer **renderer);
void process_n_update(computer_component *IR, int *x, int *y, int *x_prev, int *y_prev);
void setup(computer_component *IR, SDL_Renderer *renderer, int *x, int *y);
void render(
    SDL_Renderer **renderer,
    SDL_Texture *acumulatorTexture,
    SDL_Texture *memortTexture,
    SDL_Texture *irTexture,
    SDL_Texture *decoderTexture,
    SDL_Texture *aluTexture,
    SDL_Texture *datapathsTexture,
    SDL_Texture **textTexture,
    SDL_Texture *consoleTexture,
    SDL_Texture *cursorTexture,
    TTF_Font *font,
    computer_component *IR);
void destroy_window(SDL_Window **window, SDL_Renderer **renderer);
void setup_text(SDL_Renderer *renderer, SDL_Texture **textTexture, TTF_Font *font);

int main(int argc, char *argv[]){

    for (int i = 0; i < 7; i++)
    {
        curLines[i][0] = '\0';
    }
    //file input handling
    for(int i=20;i<4076; i++){
        strcpy(instructions[i],"0");
    }

    char lines[4076][128]; // Sring array for inputing lines of .txt file
    int i = 0, a, b, halt = 1, count = 0;
    char ch;

    // Block for opening input .txt file (name passed through command)
    FILE *fInput = fopen("../input.txt", "r");
    if (fInput == NULL){
        printf("Sorry can not find the file, FILE Error!");
        exit(1);
    }

    // Block for putting all lines of .txt input into String array lines
    for (i = 0; i < 4076; i++){
        count = 0;
        ch = getc(fInput);
        while ((ch != '\n') && (ch != EOF)){
            if (count == 0) {
                while (isspace(ch)){
                    ch = getc(fInput); // This block eats all the leading whitespace and even empty lines
                }       
            }
            lines[i][count] = ch;
            count++;
            ch = getc(fInput);
        }
        if (ch == EOF){ // stops from iterating all 128 times..
            lines[i][count] = '\0';
            i++; //i++ in the for loop body doesnt get to execute in this scenario, so need to compensate
            break;
        }
        lines[i][count] = '\0';
    }

    // Block for parsing lines array into integer instruction
    // And putting them into Instruction Memory IM
    numLines = i;
    for (i = 0; i < numLines; i++){
        
        sscanf(lines[i], "%d %d ", &oper[i], &addres[i]);
        sprintf(instructions[i],"%d %d", oper[i], addres[i]);
    }

    printf("\n");
    fclose(fInput);

    //simulation
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    TTF_Font *font = TTF_OpenFont("acme.ttf", 24);

    computer_component IR[COMPUTER_COMPONENTS];
    game_is_running = initialize_window(&window, &renderer);
    cursor_frame = 0;
    SDL_Texture
        *acumulatorTexture,
        *memoryTexture,
        *irTexture,
        *decoderTexture,
        *aluTexture,
        *datapathsTexture,
        *consoleTexture,
        *cursorTexture;

    SDL_Surface *surface;
    SDL_Texture *textTexture[6];
    setup_text(renderer, textTexture, font);

    //texture setups
    if (1)
    {
        surface = IMG_Load("pc-mar-mdr-a.png");
        if (surface == NULL)
        {
            fprintf(stderr, "Couldt load an \"pc-mar-mdr-a.png\"\n");
            return 1;
        }
        // PC - Program Counter
        // MAR - Memory Address Register
        // MDR - Memory Data Register
        // A - Accumulator
        acumulatorTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        // MEM - Memory
        surface = IMG_Load("mem.png");
        if (surface == NULL)
        {
            fprintf(stderr, "Couldt load an \"mem.png\"\n");
            return 1;
        }
        memoryTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        // IR - Instruction Register
        surface = IMG_Load("ir.png");
        if (surface == NULL)
        {
            fprintf(stderr, "Couldt load an \"ir.png\"\n");
            return 1;
        }
        irTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        // Decoder
        surface = IMG_Load("decoder.png");
        if (surface == NULL)
        {
            fprintf(stderr, "Couldt load an \"decoder.png\"\n");
            return 1;
        }
        decoderTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        // ALU - Arithmetic Logic Unit
        surface = IMG_Load("alu.png");
        if (surface == NULL)
        {
            fprintf(stderr, "Couldt load an \"alu.png\"\n");
            return 1;
        }
        aluTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        // DATA - Data Paths
        surface = IMG_Load("data-paths.png");
        if (surface == NULL)
        {
            fprintf(stderr, "Couldt load an \"data-paths.png\"\n");
            return 1;
        }
        datapathsTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        // CONSOLE
        surface = IMG_Load("console.png");
        if (surface == NULL)
        {
            fprintf(stderr, "Couldt load an \"console.png\"\n");
            return 1;
        }
        consoleTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        // CURSOR
        surface = IMG_Load("cursor.png");
        if (surface == NULL)
        {
            fprintf(stderr, "Couldt load an \"cursor.png\"\n");
            return 1;
        }
        cursorTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
    
        int x, y, x_prev, y_prev;
        setup(IR, renderer, &x, &y);
        last_frame_time = 0;

    //GAME LOOP
    while (game_is_running){   
        //ticks and timers
        diference = (SDL_GetTicks() - last_frame_time);
        if(diference<0){
            diference=0;
        }
        int time_to_wait = FRAME_TARGET_TIME - diference;
        last_frame_time = SDL_GetTicks();
        cursor_frame += diference;
        //waitTarget timer
        if(waitTarget && diference>0){
            waitTarget += diference;
        }
        //switched timer
        if(switched && diference>0){
            switched += diference;
        }
        //gotTarget timer
        if(gotTarget && diference>0){
            gotTarget += diference;
        }
        //getTarget timer
        if(getTarget && diference>0){
            getTarget+= diference;
        }
        //pcWait timer
        if(pcWait && diference>0){
            pcWait += diference;
        }
        //skip timer
        if(skip && diference>0){
            skip += diference;
        }
        //mdrToIr timer
        if(mdrToIr && diference>0){
            mdrToIr += diference;
        }
        //marToMem timer
        if(marToMem && diference>0){
            marToMem += diference;
        }
        //halt delay
        if(halt && diference>0){
            halt += diference;
        }
        //pcInc timer
        if(pcInc && diference>0){
            pcInc += diference;
        }
        //accumTransfer timer
        if(accumTransfer && diference>0){
            accumTransfer += diference;
        }
        //pcPlusOne timer
        if(pcPlusOne && diference>0){
            pcPlusOne += diference;
        }
        //fetch timer
        if(fetch && diference>0){
            fetch += diference;
        }
        //instruction timer
        if(instructionActivating && diference>0){
            instructionActivating += diference;
        }
        //mdrToAccum timer
        if(mdrToAccum && diference>0){
        mdrToAccum += diference;
        }
        //arrived timer
        if(arrived && diference>0 ){
        arrived += diference;
        }
        //mdrToMem timer
        if(mdrToMem && diference>0 ){
        mdrToMem += diference;
        }
        //address to mar timer
        if(addressToMar && diference>0 ){
        addressToMar += diference;
        }
        //oupt timer
        if(oupt && diference>0 ){
        oupt += diference;
        }
        //accumReady timer
        if(accumReady && diference>0 ){
        accumReady += diference;
        }

        //fps cap
        if (time_to_wait > 0 && time_to_wait < FRAME_TARGET_TIME)
        {
            SDL_Delay(time_to_wait);
        }
        
        //process, update, and render calls
        if(1){
        process_n_update(IR, &x, &y, &x_prev, &y_prev);
        render(&renderer,
               acumulatorTexture,
               memoryTexture,
               irTexture,
               decoderTexture,
               aluTexture,
               datapathsTexture,
               textTexture,
               consoleTexture,
               cursorTexture,
               font,
               IR);
        }
    }
    //closing touches
    if(1){
    SDL_DestroyTexture(acumulatorTexture);
    SDL_DestroyTexture(memoryTexture);
    SDL_DestroyTexture(irTexture);
    SDL_DestroyTexture(decoderTexture);
    SDL_DestroyTexture(aluTexture);
    SDL_DestroyTexture(datapathsTexture);
    SDL_DestroyTexture(consoleTexture);
    for (int i = 0; i < 6; i++)
    {
        SDL_DestroyTexture(textTexture[i]);
    }
    SDL_DestroyTexture(cursorTexture);
    destroy_window(&window, &renderer);
    IMG_Quit();
    TTF_CloseFont(font);
    TTF_Quit();
    } 
    return 0;
}

void setup(computer_component *IR, SDL_Renderer *renderer, int *x, int *y)
{
    for(int i=0; i<4096;i++){
        strcpy(memText[i],"0");
    }
    strcpy(memText[2029],"123456789");
    strcpy(memText[1],"1");
    for(int i=20;i<numLines+20; i++){
        strcpy(memText[i], instructions[i-20]);
    }

    SDL_GetMouseState(x, y);

    // DATAPATHS
    (IR)->x = 0;
    (IR)->y = 0;
    (IR)->width = 750;
    (IR)->height = 750;

    // PC
    (IR + 1)->x = 300;
    (IR + 1)->y = 105;
    (IR + 1)->width = 160;
    (IR + 1)->height = 45;

    // MAR
    (IR + 2)->x = 300;
    (IR + 2)->y = 185;
    (IR + 2)->width = 160;
    (IR + 2)->height = 45;

    // MDR
    (IR + 3)->x = 300;
    (IR + 3)->y = 470;
    (IR + 3)->width = 170;
    (IR + 3)->height = 45;

    // A
    (IR + 4)->x = 560;
    (IR + 4)->y = 470;
    (IR + 4)->width = 160;
    (IR + 4)->height = 45;

    // MEM
    (IR + 5)->x = 170;
    (IR + 5)->y = 275;
    (IR + 5)->width = 549;
    (IR + 5)->height = 120;

    // IR
    (IR + 6)->x = 50;
    (IR + 6)->y = 470;
    (IR + 6)->width = 195;
    (IR + 6)->height = 45;

    // DECODER
    (IR + 7)->x = 20;
    (IR + 7)->y = 550;
    (IR + 7)->width = 205;
    (IR + 7)->height = 60;

    // ALU
    (IR + 8)->x = 430;
    (IR + 8)->y = 570;
    (IR + 8)->width = 205;
    (IR + 8)->height = 110;

    // CONSOLE
    (IR + 9)->x = 500;
    (IR + 9)->y = 30;
    (IR + 9)->width = 230;
    (IR + 9)->height = 180;

    // CURSOR
    (IR + 10)->x = 504;
    (IR + 10)->y = 188;
    (IR + 10)->width = 4;
    (IR + 10)->height = 20;

    // 11-16 MEMORY REGISTERS

    // zeros
    (IR + 11)->x = 173;
    (IR + 11)->y = 285;
    (IR + 11)->width = textW;
    (IR + 11)->height = textH;

    // ones
    (IR + 12)->x = 173;
    (IR + 12)->y = 349;
    (IR + 12)->width = textW;
    (IR + 12)->height = textH;

    // twos
    (IR + 13)->x = 356;
    (IR + 13)->y = 285;
    (IR + 13)->width = textW;
    (IR + 13)->height = textH;

    // threes
    (IR + 14)->x = 356;
    (IR + 14)->y = 349;
    (IR + 14)->width = textW;
    (IR + 14)->height = textH;

    // forth
    (IR + 15)->x = 539;
    (IR + 15)->y = 285;
    (IR + 15)->width = textW;
    (IR + 15)->height = textH;

    // fifth
    (IR + 16)->x = 539;
    (IR + 16)->y = 349;
    (IR + 16)->width = textW;
    (IR + 16)->height = textH;

    // register values

    //Accumulator text
    (IR + 17)->x = 570;
    (IR + 17)->y = 480;
    (IR + 17)->width = textW;
    (IR + 17)->height = textH;

    //MDR text
    (IR + 18)->x = 310;
    (IR + 18)->y = 480;
    (IR + 18)->width = textW;
    (IR + 18)->height = textH;

    //Address text
    (IR + 19)->x = 105;
    (IR + 19)->y = 480;
    (IR + 19)->width = textW;
    (IR + 19)->height = textH;

    //OP text
    (IR + 20)->x = 60;
    (IR + 20)->y = 480;
    (IR + 20)->width = textW;
    (IR + 20)->height = textH;

    //MAR text
    (IR + 21)->x = 310;
    (IR + 21)->y = 190;
    (IR + 21)->width = textW;
    (IR + 21)->height = textH;

    //PC text
    (IR + 22)->x = 310 ;
    (IR + 22)->y = 110;
    (IR + 22)->width = textW;
    (IR + 22)->height = textH;

    // 11-16 MEMORY REGISTERS
    // 23-28 MEMORY REGISTERS TEXT VALUES
    // zeros text
    (IR + 23)->x = (IR + 11)->x+10;
    (IR + 23)->y = 285;
    (IR + 23)->width = textW;
    (IR + 23)->height = textH;

    // ones text
    (IR + 24)->x = (IR + 12)->x;
    (IR + 24)->y = 349;
    (IR + 24)->width = textW;
    (IR + 24)->height = textH;

    // twos text
    (IR + 25)->x = (IR + 13)->x;
    (IR + 25)->y = 285;
    (IR + 25)->width = textW;
    (IR + 25)->height = textH;

    // threes text
    (IR + 26)->x = (IR + 14)->x;
    (IR + 26)->y = 349;
    (IR + 26)->width = textW;
    (IR + 26)->height = textH;

    // forth text
    (IR + 27)->x = (IR + 15)->x;
    (IR + 27)->y = 285;
    (IR + 27)->width = textW;
    (IR + 27)->height = textH;

    // fifth text
    (IR + 28)->x = (IR + 16)->x;
    (IR + 28)->y = 349;
    (IR + 28)->width = textW;
    (IR + 28)->height = textH;

    //Address copy
    (IR + 29)->x = 105;
    (IR + 29)->y = 480;
    (IR + 29)->width = textW;
    (IR + 29)->height = textH;

    //PC copy
    (IR + 30)->x = 310 ;
    (IR + 30)->y = 110;
    (IR + 30)->width = textW;
    (IR + 30)->height = textH;

}

void setup_text(SDL_Renderer *renderer, SDL_Texture **textTexture, TTF_Font *font)
{
    SDL_Surface *surface;
    SDL_Color textColor = {255, 255, 255};
    char string[3];
    for (int i = currMem; i < currMem + 6; i++)
    {
        sprintf(string, "%d:", i);
        surface = TTF_RenderText_Blended(font, string, textColor);
        textTexture[i] = SDL_CreateTextureFromSurface(renderer, surface);
    }
    textH = surface->h;
    textW = surface->w;
    SDL_FreeSurface(surface);
}

int initialize_window(SDL_Window **window, SDL_Renderer **renderer)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "Error initializing SDL.\n");
        return 0;
    }
    *window = SDL_CreateWindow(
        "VN Architecture Simulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    if (!(*window))
    {
        fprintf(stderr, "Error in window\n");
        return 0;
    }

    *renderer = SDL_CreateRenderer(*window, -1, 0);

    if (!(*renderer))
    {
        return 0;
    }
    return 1;
}

void destroy_window(SDL_Window **window, SDL_Renderer **renderer)
{
    SDL_DestroyRenderer(*renderer);
    SDL_DestroyWindow(*window);

    IMG_Quit();
    SDL_Quit();
}

void process_n_update(computer_component *IR, int *x, int *y, int *x_prev, int *y_prev)
{
    SDL_Event event;
    int size;
    //event - yes/no
    if (SDL_PollEvent(&event))
    {
        if (mouseUpDown && event.type == SDL_MOUSEBUTTONUP) // mouse button released
        {
            mouseUpDown = 0;
        }
        else // mouse button clicked
        {
            if (!mouseUpDown && event.type == SDL_MOUSEBUTTONDOWN)
            {
                mouseUpDown = 1;
            }
        }

        int yN;
        //event switch
        switch (event.type)
        {
        case SDL_QUIT: // Close button
            game_is_running = false;
            break;
        // Keyboard
        case SDL_KEYDOWN:

            //administrative keys input
            switch (event.key.keysym.sym)
            {
            case SDLK_n:
                speed =1000;
                break;
            case SDLK_d:
                speed =500;
                break;
            case SDLK_q:
                speed =250;
                break;
            case SDLK_m:
                speed = 45;
                break;
            case SDLK_a:
                animation ^= 1;
                break;
            case SDLK_ESCAPE:
                game_is_running = false;
                break;
            case SDLK_LSHIFT:
                fetch = 1;
                break;
            case SDLK_RETURN:
                yN = 0;
                int i = 0;
                int j = 0;
                size = (int)strlen(currInput);
                for (i = 0; i < size; i++)
                {
                    if (currInput[i] == 32)
                    {
                        yN = 1;
                        break;
                    }
                }
                //if (!inpt)
                //{
                //    if (yN || (currInput[0] == '5' && (currInput[1] == ' ' || currInput[1] == '\0')) 
                //    || (currInput[0] == '6' && (currInput[1] == ' ' || currInput[1] == '\0'))
                //    || (currInput[0] == '7' && (currInput[1] == ' ' || currInput[1] == '\0')))
                //    {
                //        //copying into our OP and ADDRESS registers
                //        for (j = 0; j < i; j++)
                //        {
                //            op[j] = currInput[j];
                //        }
                //        op[j] = '\0';
                //        for (j = i + 1; j < size; j++)
                //        {
                //            address[j - i - 1] = currInput[j];
                //        }
                //        address[j - i - 1] = '\0';
                //
                //        nOp = atoi(op);
                //        nAddress = atoi(address);
                //        if (nOp == 5 || (nOp > -1 && nOp < 16 && nAddress > -1 && nAddress < 4096))
                //        {
                //            instructionActivating = 1;
                //            for (int g = 6; g >= 0; g--)
                //            {
                //                char strr[20];
                //                for (int j = 0; j < strlen(curLines[g]); j++)
                //                {
                //                    curLines[g + 1][j] = curLines[g][j];
                //                }
                //                curLines[g + 1][strlen(curLines[g])] = '\0';
                //            }
                //            for (int j = 0; j < strlen(currInput); j++)
                //            {
                //                curLines[0][j] = currInput[j];
                //            }
                //            curLines[0][strlen(currInput)] = '\0';
                //            strcpy(currInput, "");
                //        }
                //    }
                //}
                //else
                ////else inpt (yes)
                //{
                if (!yN && strlen(currInput)>0)
                {
                    strcpy(address, currInput);
                    inpt =0;
                    accumTransfer = 1;
                    
                    for (int g = 6; g >= 0; g--)
                    {
                        char strr[20];
                        for (int j = 0; j < strlen(curLines[g]); j++)
                        {
                            curLines[g + 1][j] = curLines[g][j];
                        }
                        curLines[g + 1][strlen(curLines[g])] = '\0';
                    }
                    for (int j = 0; j < size; j++)
                    {
                        curLines[0][j] = currInput[j];
                    }
                    curLines[0][size] = '\0';
                    strcpy(currInput, "");
                }
                //}

                break;
            case SDLK_BACKSPACE:
                if (strlen(currInput) < 1)
                {
                    break;
                }
                currInput[strlen(currInput) - 1] = '\0';
                break;
            }
            //number input cap
            if (!stdOut || strlen(currInput) > 8)
            {
                break;
            }
            // number keys input
            switch (event.key.keysym.sym)
            {
            case SDLK_0:
                strcat(currInput, "0");
                break;
            case SDLK_KP_0:
                strcat(currInput, "0");
                break;
            case SDLK_1:
                strcat(currInput, "1");
                break;
            case SDLK_2:
                strcat(currInput, "2");
                break;
            case SDLK_3:
                strcat(currInput, "3");
                break;
            case SDLK_4:
                strcat(currInput, "4");
                break;
            case SDLK_5:
                strcat(currInput, "5");
                break;
            case SDLK_6:
                strcat(currInput, "6");
                break;
            case SDLK_7:
                strcat(currInput, "7");
                break;
            case SDLK_8:
                strcat(currInput, "8");
                break;
            case SDLK_9:
                strcat(currInput, "9");
                break;
            case SDLK_KP_1:
                strcat(currInput, "1");
                break;
            case SDLK_KP_2:
                strcat(currInput, "2");
                break;
            case SDLK_KP_3:
                strcat(currInput, "3");
                break;
            case SDLK_KP_4:
                strcat(currInput, "4");
                break;
            case SDLK_KP_5:
                strcat(currInput, "5");
                break;
            case SDLK_KP_6:
                strcat(currInput, "6");
                break;
            case SDLK_KP_7:
                strcat(currInput, "7");
                break;
            case SDLK_KP_8:
                strcat(currInput, "8");
                break;
            case SDLK_KP_9:
                strcat(currInput, "9");
                break;
            
            case SDLK_SPACE:
                yN = 0;
                for (int i = 0; i < strlen(currInput); i++)
                {
                    if (currInput[i] == 32)
                    {
                        yN = 1;
                        break;
                    }
                }
                if (yN)
                {
                    break;
                }
                strcat(currInput, " ");
                break;
            }
            break;
        //mouse input
        case SDL_MOUSEBUTTONDOWN:
            SDL_GetMouseState(x, y);
            if (*x > 500 && *x < 730 && *y > 50 && *y < 210)
            {
                stdOut = 1;
            }
            else
            {
                stdOut = 0;
            }
            break;
        }
        // scrolling
        if (mouseUpDown)
        {
            *x_prev = *x;
            *y_prev = *y;
            int delta_x, delta_y;
            SDL_GetMouseState(x, y);
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                delta_x = 0;
                delta_y = 0;
            }
            else
            {
                delta_x = *x - *x_prev;
                delta_y = *y - *y_prev;
            }

            for (int i = 0; i < 9; i++)
            {
                (IR + i)->x = (IR + i)->x + delta_x;
                (IR + i)->y = (IR + i)->y + delta_y;
            }
            // 10 AND 11 are console and cursor
            for (int i = 11; i < COMPUTER_COMPONENTS; i++)
            {
                (IR + i)->x = (IR + i)->x + delta_x;
                (IR + i)->y = (IR + i)->y + delta_y;
            }
        }
    }
    //accumTransfer state
    if(accumTransfer > speed){
        accumTransfer = 0;
        strcpy(accumText, address );
        fetch = 1;
    }
    //pc plus one state
    if(pcPlusOne > speed/2){
        strcpy(marText, pcText);
        pcPlusOne = 0;
        pcInc=1;
    }
    if(pcInc> speed/2){
        pcWait =1;
        pcInc =0;
        char str[20];
        sprintf(str, "%d", atoi(pcText)+1);
        strcpy(pcText, str);
    }
    if(pcWait> speed/2){
        getTarget =1;
        pcWait = 0;
    }
    //fetch state
    if(fetch > speed){
        fetch = 0; 
        pcPlusOne = 1;
    }
    //input state
    if(accumReady>speed){
        strcpy(pcText, addressText);
        
        accumReady = 0;
        inpt = 0;
        fetch = 1;
    }
    //address to mar
    if(addressToMar>speed){     
        addressToMar =0;
        if(store || load || add || subt){
            strcpy(marText, addressText);
            waitTarget =1;
        }
    } 
    if(waitTarget>speed/2){
        waitTarget =0;
        getTarget=1;
    }  

    if(getTarget > speed/2)
    {
        getTarget =0;
        gotTarget=1;
        if(store || load || add || subt){
            target_address = atoi(marText);
            if (currMem >= (target_address - (target_address % 6)) && currMem < (target_address - (target_address % 6) + 6)){
            gotTarget = 1;
        }
        }
            target_address = atoi(marText);
            if (currMem >= (target_address - (target_address % 6)) && currMem < (target_address - (target_address % 6) + 6)){
            gotTarget = 1;
        }
    }
    if(gotTarget > speed/2){
        gotTarget =0;
        arrived=1;
    }
    //arriving to target mem cell
    //marToMem
    if(marToMem > speed/2){
        if(store){
            mdrToMem = 1;
        }
        else if(load || add || subt){
            mdrToAccum = 1;
        }
        else{//fetch
            mdrToIr = 1;
        }
        marToMem = 0;
    }
    if(arrived > speed){
        if(store){
            strcpy(mdrText, accumText);
            arrived = 0;
            marToMem = 1;
        }
        else if(load || add || subt){
            strcpy(mdrText, memText[atoi(marText)]);
            arrived = 0;
            marToMem = 1;
            
        }
        else{//fetch
            arrived = 0;
            strcpy(mdrText, memText[atoi(marText)]);
            marToMem = 1;
        }
    }
    //mdrToIr timer
    if(mdrToIr > speed){
            mdrToIr =0;
            fetch = 0;
            strcpy(currInput, mdrText);
            int yN = 0;
            int i = 0;
            int j = 0;
            size = (int)strlen(currInput);
            for (i = 0; i < size; i++)
            {
                if (currInput[i] == 32)
                {
                    yN = 1;
                    break;
                }
            }
            if (yN || (currInput[0] == '5' && (currInput[1] == ' ' || currInput[1] == '\0')) 
            || (currInput[0] == '6' && (currInput[1] == ' ' || currInput[1] == '\0'))
            || (currInput[0] == '7' && (currInput[1] == ' ' || currInput[1] == '\0'))
            || (currInput[0] == '9' && (currInput[1] == ' ' || currInput[1] == '\0'))
            || (currInput[0] == '1' && currInput[0] == '0' && (currInput[2] == ' ' || currInput[2] == '\0'))
            || (currInput[0] == '1' && currInput[0] == '1' && (currInput[2] == ' ' || currInput[2] == '\0')))
            {
                //copying into our OP and ADDRESS registers
                for (j = 0; j < i; j++)
                {
                    op[j] = currInput[j];
                }
                op[j] = '\0';
                for (j = i + 1; j < size; j++)
                {
                    address[j - i - 1] = currInput[j];
                }
                address[j - i - 1] = '\0';
                nOp = atoi(op);
                nAddress = atoi(address);
                if (nOp == 5 || (nOp > -1 && nOp < 16 && nAddress > -1 && nAddress < 4096))
                {
                    strcpy(opText, op);
                    strcpy(addressText, address);
                    instructionActivating = 1;
                    //for (int g = 6; g >= 0; g--)
                    //{
                    //    char strr[20];
                    //    for (int j = 0; j < strlen(curLines[g]); j++)
                    //    {
                    //        curLines[g + 1][j] = curLines[g][j];
                    //    }
                    //    curLines[g + 1][strlen(curLines[g])] = '\0';
                    //}
                    //for (int j = 0; j < strlen(currInput); j++)
                    //{
                    //    curLines[0][j] = currInput[j];
                    //}
                    //curLines[0][strlen(currInput)] = '\0';
                    strcpy(currInput, "");
                }
            }
    }
    //mdr to accumulator timer
    if(mdrToAccum > speed){
        if(load){
        strcpy(accumText, mdrText);
        fetch =1;
        load = 0;
        mdrToAccum = 0;
        }
        else if(add){
        char str[13];
        sprintf(str,"%d",atoi(mdrText)+atoi(accumText));
        strcpy(accumText, str);
        fetch =1;
        add = 0;
        mdrToAccum = 0;
        }
        else if(subt){
        char str[13];
        sprintf(str,"%d",atoi(accumText)-atoi(mdrText));
        strcpy(accumText, str);
        fetch =1;
        subt = 0;
        mdrToAccum = 0;
        }
    }
    if(mdrToMem > speed){
        if(store){
            strcpy(memText[atoi(marText)],mdrText);
            mdrToMem = 0;
            fetch = 1;
            store = 0;
        }
    }
    //output timer
    if(oupt > speed){
        for (int g = 6; g >= 0; g--)
        {
            char strr[20];
            for (int j = 0; j < strlen(curLines[g]); j++)
            {
                curLines[g + 1][j] = curLines[g][j];
            }
            curLines[g + 1][strlen(curLines[g])] = '\0';
        }
        strcpy(curLines[0], accumText);
        oupt = 0;
        fetch = 1;

    }
    //skip state
    if(skip > speed){
        skip = 0;
        fetch = 1;
        char str[20];
        sprintf(str, "%d", atoi(pcText)+1);
        if(nOp ==9){ 
            if(atoi(accumText)==0){
                strcpy(pcText, str);
                fetch =0;
                switched =1;
            }
        }
        if(nOp ==10){
            if(atoi(accumText)>0){
                strcpy(pcText, str);
                fetch =0;
                switched =1;
            }
        }
        if(nOp ==11){
            if(atoi(accumText)<0){
                strcpy(pcText, str);
                fetch =0;
                switched =1;
            }
        }
    }
    if(switched>speed){
        switched =0;
        fetch =1;
    }
    //instructions
    if(halt > HALT_DELAY){
        game_is_running = false;
        halt = 0;
    }  
    if (instructionActivating> speed)
    {
        strcpy(opText, op);
        strcpy(addressText, address);

        if (nOp == 1){
            
            load = 1;
            addressToMar = 1;
        }

        if (nOp == 2){
            
            add = 1;
            addressToMar = 1;
        }

        if (nOp == 3)
        {
            
            store = 1;
            addressToMar = 1;
        } 

        if (nOp == 4)
        {
            
            subt = 1;
            addressToMar = 1;
        }

        if (nOp == 5)
        {
            inpt = 1;
            for (int g = 6; g >= 0; g--)
            {
                char strr[20];
                for (int j = 0; j < strlen(curLines[g]); j++)
                {
                    curLines[g + 1][j] = curLines[g][j];
                }
                curLines[g + 1][strlen(curLines[g])] = '\0';
            }
            strcpy(curLines[0], "Enter input:");
        }

        if (nOp == 6)
        {   
            oupt = 1;
            for (int g = 6; g >= 0; g--)
            {
                char strr[20];
                for (int j = 0; j < strlen(curLines[g]); j++)
                {
                    curLines[g + 1][j] = curLines[g][j];
                }
                curLines[g + 1][strlen(curLines[g])] = '\0';
            }
            strcpy(curLines[0], "OUTPUT:");
        }

        if (nOp == 7)
        {
            halt = 1;
            
            // closing touches
        }

        if (nOp == 8)
        {   
            accumReady = 1;
        }

        if (nOp == 9)
        {   
            skip = 1;
        }

        if (nOp == 10)
        {   
            skip = 1;
        }

        if (nOp == 11)
        {   
            skip = 1;
        }

        instructionActivating = 0;
    }
}

void render(
    SDL_Renderer **renderer,
    SDL_Texture *acumulatorTexture,
    SDL_Texture *memortTexture,
    SDL_Texture *irTexture,
    SDL_Texture *decoderTexture,
    SDL_Texture *aluTexture,
    SDL_Texture *datapathsTexture,
    SDL_Texture **textTexture,
    SDL_Texture *consoleTexture,
    SDL_Texture *cursorTexture,
    TTF_Font *font,
    computer_component *IR)
{
    SDL_SetRenderDrawColor(*renderer, 10, 10, 40, 255);
    SDL_RenderClear(*renderer);
    SDL_Color animColor = {76, 187, 23};

    SDL_Rect OBJ_rect;

    // PC, MAR, MDR, A
    for (int i = 1; i < 5; i++)
    {
        OBJ_rect.x = (IR + i)->x;
        OBJ_rect.y = (IR + i)->y;
        OBJ_rect.w = (IR + i)->width;
        OBJ_rect.h = (IR + i)->height;

        SDL_RenderCopy(*renderer, acumulatorTexture, NULL, &OBJ_rect);
    }
    // Memory, IR, Decoder, ALU, Data Paths
    if(1){

    // Memory
    OBJ_rect.x = (IR + 5)->x;
    OBJ_rect.y = (IR + 5)->y;
    OBJ_rect.w = (IR + 5)->width;
    OBJ_rect.h = (IR + 5)->height;
    SDL_RenderCopy(*renderer, memortTexture, NULL, &OBJ_rect);

    // IR
    OBJ_rect.x = (IR + 6)->x;
    OBJ_rect.y = (IR + 6)->y;
    OBJ_rect.w = (IR + 6)->width;
    OBJ_rect.h = (IR + 6)->height;
    SDL_RenderCopy(*renderer, irTexture, NULL, &OBJ_rect);

    // Decoder
    OBJ_rect.x = (IR + 7)->x;
    OBJ_rect.y = (IR + 7)->y;
    OBJ_rect.w = (IR + 7)->width;
    OBJ_rect.h = (IR + 7)->height;
    SDL_RenderCopy(*renderer, decoderTexture, NULL, &OBJ_rect);

    // ALU
    OBJ_rect.x = (IR + 8)->x;
    OBJ_rect.y = (IR + 8)->y;
    OBJ_rect.w = (IR + 8)->width;
    OBJ_rect.h = (IR + 8)->height;
    SDL_RenderCopy(*renderer, aluTexture, NULL, &OBJ_rect);

    // Data Paths
    OBJ_rect.x = (IR)->x;
    OBJ_rect.y = (IR)->y;
    OBJ_rect.w = (IR)->width;
    OBJ_rect.h = (IR)->height;
    SDL_RenderCopy(*renderer, datapathsTexture, NULL, &OBJ_rect);
    }

    //pulling up the memory cell. to target address
    if (currMem < (target_address - (target_address % 6)) || currMem >= (target_address - (target_address % 6) + 6))
    {
        int dif = target_address - currMem;
        if (abs(dif) < 60)
        {
            if (dif > 0)
            {
                currMem += 6;
            }
            else
            {
                currMem -= 6;
            }
        }
        else
        {
            if (dif > 0)
            {
                currMem += 60;
            }
            else
            {
                currMem -= 60;
            }
        }
        if (currMem >= (target_address - (target_address % 6)) && currMem < (target_address - (target_address % 6) + 6)){
            gotTarget = 1;
        }
    }
    
    // Memory indecies
    SDL_Rect textRect;
    SDL_Surface *surface;
    SDL_Color textColor = {255, 255, 255};
    char string[6];
    //mem indecies and their values
    int indexW;
    for (int i = 11; i < 17; i++)
    {
        sprintf(string, "%d:", i - 11 + currMem);
        surface = TTF_RenderText_Blended(font, string, textColor);
        textTexture[i - 11] = SDL_CreateTextureFromSurface(*renderer, surface);

        textRect.x = (IR + i)->x;
        textRect.y = (IR + i)->y;
        textRect.w = surface->w;
        textRect.h = surface->h;
        SDL_RenderCopy(*renderer, textTexture[i - 11], NULL, &textRect);
        indexW = surface->w;
        SDL_FreeSurface(surface);
        

        //memory value at that index
        if(strlen(memText[currMem+i-11]) == 0){
            continue;
        }
        surface = TTF_RenderText_Blended(font, memText[currMem+i-11], textColor);
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);
        //(IR + 10)->x = 504;
        //(IR + 10)->y = 188;
        OBJ_rect.x = (IR + i + 12)->x +indexW+5;
        OBJ_rect.y = (IR + i + 12)->y;
        OBJ_rect.w = surface->w;
        OBJ_rect.h = surface->h;
        SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(inputTexture);
    };   
    
    //memory value copy load add subt
        if(animation && arrived && !store){
        surface = TTF_RenderText_Blended(font, memText[atoi(marText)], animColor);
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);

        OBJ_rect.x = ((IR + (atoi(marText)%6+ 23))->x+indexW+5) + arrived*((IR+18)->x - (IR + (atoi(marText)%6+ 23))->x-indexW-5)/speed;
        OBJ_rect.y = ((IR + (atoi(marText)%6+ 23))->y) + arrived*((IR+18)->y - (IR + (atoi(marText)%6+ 23))->y)/speed;
        OBJ_rect.w = surface->w;
        OBJ_rect.h = surface->h;
        SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(inputTexture);
        }
    //memory copy 1    
        if(gotTarget && animation){
        surface = TTF_RenderText_Blended(font,marText, animColor);
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);

        OBJ_rect.x = ((IR + (atoi(marText)%6+ 11))->x);
        OBJ_rect.y = ((IR + (atoi(marText)%6+ 11))->y);
        OBJ_rect.w = surface->w;
        OBJ_rect.h = surface->h;
        SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(inputTexture);
        }
    //memory value copy2  store
        if(animation && arrived && store){
        surface = TTF_RenderText_Blended(font, accumText, animColor);
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);

        OBJ_rect.x = ((IR + 17)->x) + arrived*((IR+18)->x - (IR + 17)->x)/speed;
        OBJ_rect.y = ((IR + 17)->y) + arrived*((IR+18)->y - (IR + 17)->y)/speed;
        OBJ_rect.w = surface->w;
        OBJ_rect.h = surface->h;
        SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(inputTexture);
        }
    

    // Console  
    OBJ_rect.x = (IR + 9)->x;
    OBJ_rect.y = (IR + 9)->y;
    OBJ_rect.w = (IR + 9)->width;
    OBJ_rect.h = (IR + 9)->height;
    SDL_RenderCopy(*renderer, consoleTexture, NULL, &OBJ_rect);

    // cursor animation

    if (cursor_frame > 1000)
    {
        cursor_frame = 0;
    }
    //console lines text
    for (int i = 0; i < 7; i++)
    {
        if (strlen(curLines[i]) == 0)
        {
            break;
        }
        surface = TTF_RenderText_Blended(font, curLines[i], textColor);
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);
        //(IR + 10)->x = 504;
        //(IR + 10)->y = 188;
        OBJ_rect.x = 504;
        OBJ_rect.y = 162 - i * 20;
        OBJ_rect.w = surface->w;
        OBJ_rect.h = surface->h;
        SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(inputTexture);
    }

        if (animation && accumTransfer)
        {
        surface = TTF_RenderText_Blended(font, curLines[0], animColor);
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);
        //(IR + 10)->x = 504;
        //(IR + 10)->y = 188;
        OBJ_rect.x = 504+accumTransfer*((IR+17)->x-504)/speed;
        OBJ_rect.y = 162+accumTransfer*((IR+17)->y-162)/speed;
        OBJ_rect.w = surface->w;
        OBJ_rect.h = surface->h;
        SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(inputTexture);
        }

    //accumulator text
    if (strlen(accumText) != 0)
        {
        surface = TTF_RenderText_Blended(font, accumText, textColor);
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);
        OBJ_rect.x = (IR + 17)->x;
        OBJ_rect.y = (IR + 17)->y;
        OBJ_rect.w = surface->w;
        OBJ_rect.h = surface->h;
        SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(inputTexture);
        }
    //accumulator text copy
    if (oupt && animation)
        {
        surface = TTF_RenderText_Blended(font, accumText, animColor);
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);
        //504;
        //182;
        OBJ_rect.x = (IR + 17)->x+oupt*(504-(IR + 17)->x)/speed;
        OBJ_rect.y = (IR + 17)->y+oupt*(162-(IR + 17)->y)/speed;
        OBJ_rect.w = surface->w;
        OBJ_rect.h = surface->h;
        SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(inputTexture);
        }
    //accumulator text copy 2
    if (skip)
        {
        surface = TTF_RenderText_Blended(font, accumText, animColor);
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);
        OBJ_rect.x = (IR + 17)->x;
        OBJ_rect.y = (IR + 17)->y;
        OBJ_rect.w = surface->w;
        OBJ_rect.h = surface->h;
        SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(inputTexture);
        }
    //mdr text
    if (strlen(mdrText) != 0)
        {
        surface = TTF_RenderText_Blended(font, mdrText, textColor);
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);
        //(IR + 10)->x = 504;
        //(IR + 10)->y = 188;
        OBJ_rect.x = (IR + 18)->x;
        OBJ_rect.y = (IR + 18)->y;
        OBJ_rect.w = surface->w;
        OBJ_rect.h = surface->h;
        SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(inputTexture);
        }
    //mdr copy
    if (mdrToIr && animation)
        {
        surface = TTF_RenderText_Blended(font, mdrText, animColor);
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);
        //(IR + 10)->x = 504;
        //(IR + 10)->y = 188;
        OBJ_rect.x = (IR + 18)->x+mdrToIr * ((IR+20)->x-(IR + 18)->x)/speed;
        OBJ_rect.y = (IR + 18)->y+mdrToIr * ((IR+20)->y-(IR + 18)->y)/speed;
        OBJ_rect.w = surface->w;
        OBJ_rect.h = surface->h;
        SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(inputTexture);
        }
    //mdr copy 2
    if (mdrToAccum && animation)
        {
        surface = TTF_RenderText_Blended(font, mdrText, animColor);
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);
        //(IR + 10)->x = 504;
        //(IR + 10)->y = 188;
        OBJ_rect.x = (IR + 18)->x+mdrToAccum * ((IR+17)->x-(IR + 18)->x)/speed;
        OBJ_rect.y = (IR + 18)->y+mdrToAccum * ((IR+17)->y-(IR + 18)->y)/speed;
        OBJ_rect.w = surface->w;
        OBJ_rect.h = surface->h;
        SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(inputTexture);
        }
    //mdr copy 3
    if (mdrToMem && animation)
        {
        surface = TTF_RenderText_Blended(font, mdrText, animColor);
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);
      
        OBJ_rect.x = (IR + 18)->x+mdrToMem * ((IR + (atoi(marText)%6+ 23))->x+indexW+5-(IR+18)->x)/speed;
        OBJ_rect.y = (IR + 18)->y+mdrToMem * ((IR + (atoi(marText)%6+ 23))->y-(IR+18)->y)/speed;
        OBJ_rect.w = surface->w;
        OBJ_rect.h = surface->h;
        SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(inputTexture);
        }
    
    //address text
    if (strlen(addressText) != 0)
        {
        surface = TTF_RenderText_Blended(font, addressText, textColor);
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);
        //(IR + 10)->x = 504;
        //(IR + 10)->y = 188;
        OBJ_rect.x = (IR + 19)->x;
        OBJ_rect.y = (IR + 19)->y;
        OBJ_rect.w = surface->w;
        OBJ_rect.h = surface->h;
        SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(inputTexture);
        }
    // address copies
    if (animation && accumReady)
        {
        surface = TTF_RenderText_Blended(font, addressText, animColor);
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);
            if(accumReady<speed*370/570){
            (IR + 29)->y=(IR + 19)->y-accumReady*570/(speed);
            (IR + 29)->x=(IR + 19)->x;
            }
            else{
            (IR + 29)->y=(IR + 22)->y;
            (IR + 29)->x=(IR + 19)->x+(accumReady-speed*370/570)*570/(speed);
            }
        

        OBJ_rect.x = (IR + 29)->x;
        OBJ_rect.y = (IR + 29)->y;
        OBJ_rect.w = surface->w;
        OBJ_rect.h = surface->h;
        SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(inputTexture);
        }
    if (animation && addressToMar)
        {
        surface = TTF_RenderText_Blended(font, addressText, animColor);
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);
            if(addressToMar<speed*290/495){
            (IR + 29)->y=(IR + 19)->y-addressToMar*495/(speed);
            (IR + 29)->x=(IR + 19)->x;
            }
            else{
            (IR + 29)->y=(IR + 21)->y;
            (IR + 29)->x=(IR + 19)->x+(addressToMar-speed*290/495)*495/(speed);
            }
        

        OBJ_rect.x = (IR + 29)->x;
        OBJ_rect.y = (IR + 29)->y;
        OBJ_rect.w = surface->w;
        OBJ_rect.h = surface->h;
        SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(inputTexture);
        }

    //op text
    if (strlen(opText) != 0)
        {
        surface = TTF_RenderText_Blended(font, opText, textColor);
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);
        //(IR + 10)->x = 504;
        //(IR + 10)->y = 188;
        OBJ_rect.x = (IR + 20)->x;
        OBJ_rect.y = (IR + 20)->y;
        OBJ_rect.w = surface->w;
        OBJ_rect.h = surface->h;
        SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(inputTexture);
        }
    
    //mar text
    if (strlen(marText) != 0)
        {
        surface = TTF_RenderText_Blended(font, marText, textColor);
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);
        //(IR + 10)->x = 504;
        //(IR + 10)->y = 188;
        OBJ_rect.x = (IR + 21)->x;
        OBJ_rect.y = (IR + 21)->y;
        OBJ_rect.w = surface->w;
        OBJ_rect.h = surface->h;
        SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(inputTexture);
        }
        //TOFIX: atoi(marText)-currMem
    //mar text copy
    if (animation && (getTarget))
        {
        surface = TTF_RenderText_Blended(font, marText, animColor);
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);
        OBJ_rect.x = (IR + 21)->x +getTarget*2*((IR + (atoi(marText)%6+11))->x-(IR + 21)->x)/speed;
        OBJ_rect.y = (IR + 21)->y + getTarget*2*((IR + (atoi(marText)%6+11))->y-(IR + 21)->y)/speed ;
        OBJ_rect.w = surface->w;
        OBJ_rect.h = surface->h;
        SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(inputTexture);
        }

    //pc text
    if (strlen(pcText) != 0)
        {
        surface = TTF_RenderText_Blended(font, pcText, textColor);
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);
        //(IR + 10)->x = 504;
        //(IR + 10)->y = 188;
        OBJ_rect.x = (IR + 22)->x;
        OBJ_rect.y = (IR + 22)->y;
        OBJ_rect.w = surface->w;
        OBJ_rect.h = surface->h;
        SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(inputTexture);
        }
    //pc copy 2
    if ((pcWait)&& !(load || store || add|| subt) && animation)
        {
        surface = TTF_RenderText_Blended(font, pcText, animColor);
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);
        OBJ_rect.x = (IR + 22)->x;
        OBJ_rect.y = (IR + 22)->y;
        OBJ_rect.w = surface->w;
        OBJ_rect.h = surface->h;
        SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(inputTexture);
        }

    //pc copy 3
    if (pcPlusOne && animation)
        {
        surface = TTF_RenderText_Blended(font, pcText, animColor);
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);
        
        
        (IR + 30)->y=(IR + 22)->y+pcPlusOne*(80)*2/(speed);
        (IR + 30)->x=(IR + 22)->x;
            
        
        OBJ_rect.x = (IR + 30)->x;
        OBJ_rect.y = (IR + 30)->y;
        OBJ_rect.w = surface->w;
        OBJ_rect.h = surface->h;
        SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(inputTexture);
        }
    //pc text copy 4
    if (switched)
        {
        surface = TTF_RenderText_Blended(font, pcText, animColor);
        SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);
        //(IR + 10)->x = 504;
        //(IR + 10)->y = 188;
        OBJ_rect.x = (IR + 22)->x;
        OBJ_rect.y = (IR + 22)->y;
        OBJ_rect.w = surface->w;
        OBJ_rect.h = surface->h;
        SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(inputTexture);
        }
    cursor_frame++;
    // Input
    if (strlen(currInput) == 0)
    {
        // Cursor
        if (!stdOut || cursor_frame < 500)
        {
            OBJ_rect.x = (IR + 10)->x;
            OBJ_rect.y = (IR + 10)->y;
            OBJ_rect.w = (IR + 10)->width;
            OBJ_rect.h = (IR + 10)->height;
            SDL_RenderCopy(*renderer, cursorTexture, NULL, &OBJ_rect);
        }

        SDL_RenderPresent(*renderer);
        return;
    }

    surface = TTF_RenderText_Blended(font, currInput, textColor);
    SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(*renderer, surface);
    OBJ_rect.x = 504;
    OBJ_rect.y = 182;
    OBJ_rect.w = surface->w;
    OBJ_rect.h = surface->h;
    SDL_RenderCopy(*renderer, inputTexture, NULL, &OBJ_rect);
    // Cursor + animation of blinking
    if (!stdOut || cursor_frame < 500)
    {
        OBJ_rect.x = (IR + 10)->x + surface->w;
        OBJ_rect.y = (IR + 10)->y;
        OBJ_rect.w = (IR + 10)->width;
        OBJ_rect.h = (IR + 10)->height;
        SDL_RenderCopy(*renderer, cursorTexture, NULL, &OBJ_rect);
    }
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(inputTexture);

    // swapping the buffers
    SDL_RenderPresent(*renderer);
    // todo
}
