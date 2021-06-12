#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1000;

const int TILE_ROWS = 32;
const int TILE_COLS = 50;
char tilemap[TILE_ROWS][TILE_COLS];

const int TILE_SIZE = 40;

typedef enum {
    DOWN,
    FLAT,
    UP,
    NUM_TILE_TYPES
} TILE_TYPE;

int main() {
    // INIT STUFF

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *tileset_texture_atlas;
    SDL_Rect texture_atlas_clips[ NUM_TILE_TYPES ];
    texture_atlas_clips[ DOWN ].x = 0;
    texture_atlas_clips[ DOWN ].y = 0;
    texture_atlas_clips[ DOWN ].w = TILE_SIZE;
    texture_atlas_clips[ DOWN ].h = TILE_SIZE;
    texture_atlas_clips[ FLAT ].x = TILE_SIZE * 1 ;
    texture_atlas_clips[ FLAT ].y = 0;
    texture_atlas_clips[ FLAT ].w = TILE_SIZE;
    texture_atlas_clips[ FLAT ].h = TILE_SIZE;
    texture_atlas_clips[ UP ].x = TILE_SIZE * 2;
    texture_atlas_clips[ UP ].y = 0;
    texture_atlas_clips[ UP ].w = TILE_SIZE;
    texture_atlas_clips[ UP ].h = TILE_SIZE;

    if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf( stderr, "Error %s\n", SDL_GetError() );
        exit( EXIT_FAILURE );
    }
    if (! ( IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG ) ) {
        fprintf( stderr, "Error %s\n ", IMG_GetError() );
        exit( EXIT_FAILURE );
    }

    window = SDL_CreateWindow( "RAILS", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
    if ( window == NULL ) {
        fprintf( stderr, "Error %s\n ", SDL_GetError() );
        exit( EXIT_FAILURE );
    }

    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
    if (renderer == NULL ) {
        fprintf( stderr, "Error %s\n ", SDL_GetError() );
        exit( EXIT_FAILURE );
    }

    // LOAD IMAGE as texture
    SDL_Surface *surface = IMG_Load( "res/img/tileset.png" );
    if( surface == NULL ) {
        fprintf( stderr, "Error %s\n ", IMG_GetError() );
        exit( EXIT_FAILURE );
    }

    tileset_texture_atlas = SDL_CreateTextureFromSurface( renderer, surface );
    if( tileset_texture_atlas == NULL ) {
        fprintf( stderr, "Error %s\n ", SDL_GetError() );
        exit( EXIT_FAILURE );
    }

    // LOAD TILEMAP FILE

    memset( tilemap, ' ', TILE_ROWS * TILE_COLS );


    FILE *f = fopen("res/tilemaps/chunk1.tm", "r");
    if( f == NULL ) {
        fprintf(stderr, "error reading file\n");
    }
    // logic kinda flaky at the moment. need to advance to next line if max line length is reached.
    char line[51]; // max TILE_COLS + \n
    int row = 0;
    int col = 0;
    int line_idx = 0;
    while( fgets(line, 51, f) != NULL ) {
        while( line[ line_idx ] != '\n' && ! (line_idx >= 50) ) {
            tilemap[ row ][ line_idx ] = line[ line_idx ];
            line_idx++;
        }
        row++;
        line_idx = 0;
    }


    // PREPARE VARIABLES FOR LOOP
    SDL_Event event;
    int quit = 0;

    // delta time - frame rate independent movement
    float max_delta_time = 1 / 60.0;
    float previous_frame_ticks = SDL_GetTicks() / 1000.0;

    while (!quit) {

        // semi-fixed timestep
        float current_frame_ticks = SDL_GetTicks() / 1000.0;
        float delta_time = current_frame_ticks - previous_frame_ticks;
        previous_frame_ticks = current_frame_ticks;
        // adjust for any pauses, debugging breaks, etc
        delta_time = delta_time < max_delta_time ?  delta_time : max_delta_time;

        // EVENTS
        while (SDL_PollEvent( &event ) != 0 ) {
            if( event.type == SDL_QUIT ) {
                quit = 1;
            }

        }
        if(quit) break;

        // RENDER
        SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
        SDL_RenderClear( renderer ); // set bg color 

        
        // tilemap render
        SDL_Rect dest_rect = {0, 0, 0, 0};
        SDL_Rect src_rect = {0, 0, 0 ,0};
        for( int row = 0; row < TILE_ROWS; ++row ) {
            for( int col = 0; col < TILE_COLS; ++col ) {
                if( tilemap[ row ][ col ]!= '#' && tilemap[ row ][ col ] != '\\' && tilemap[ row ][ col ] != '/' ) {
                    continue;
                }
                switch( tilemap[ row ][ col ] ) {
                    case '\\':
                        src_rect = texture_atlas_clips[ DOWN ];
                        break;
                    case '#':
                        src_rect = texture_atlas_clips[ FLAT ];
                        break;
                    case '/':
                        src_rect = texture_atlas_clips[ UP ];
                        break;
                }
                dest_rect.x = col * TILE_SIZE;
                dest_rect.y = row * TILE_SIZE;
                dest_rect.w  = TILE_SIZE;
                dest_rect.h  = TILE_SIZE;

                SDL_RenderCopy( renderer, tileset_texture_atlas, &src_rect, &dest_rect );
            }
            
        }

        // draw grid
        SDL_SetRenderDrawColor( renderer, 50,50,50,255);
        for ( int y = 0; y < SCREEN_HEIGHT; y+= TILE_SIZE ) {
            SDL_RenderDrawLine( renderer, 0, y, SCREEN_WIDTH, y);
        }
        for ( int x = 0; x < SCREEN_WIDTH; x+= TILE_SIZE) {
            SDL_RenderDrawLine( renderer, x, 0, x, SCREEN_HEIGHT );
        }


        SDL_RenderPresent( renderer ); 
        

        SDL_Delay( 5 );
    }
    printf("reached end\n");
    fclose( f );
    exit(EXIT_SUCCESS);
    
}