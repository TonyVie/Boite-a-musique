#include "mod_Menu.h"
#include "mod_Led.h"
#include "mod_AudioPlayer.h"

#include <MenuSystem.h>
#include <timer.h>

auto timer = timer_create_default();
uintptr_t menuTimeoutTask;

bool quitMenu(void *argument /* optional argument given to in/at/every */) ;

uint32_t currentColor;
void onItemSelected(MenuComponent* p_menu_component);

class PlaylistMenuItem: public MenuItem
{
public:
    PlaylistMenuItem(const char* name
                   , uint32_t color_
                   , int playlistIndex_
                   , SelectFnPtr select_fn)
        : MenuItem(name, select_fn), color(color_), playlistIndex(playlistIndex_),
          playing(false)
    {
        
    }

public:
    bool isPlaying() const
    {
        return playing;
    }
    
    void setPlaying(const bool shouldBePlaying = true)
    {
        playing = shouldBePlaying;
        Serial.print("setPlaying ");
        Serial.print(playlistIndex);
        Serial.println(playing ? ":yes" : ":no");
        
        setAudioPlayerPlaying(playing);
        if(playing)
            loadAudioPlayerFolder(playlistIndex);

        if(playing)
            animateLed(color, rotate);
        else
            animateLed(color, blink);
    }

public:
    int playlistIndex;
    uint32_t color;

protected:
    bool playing;
};

class MenuRenderer : public MenuComponentRenderer {
public:
    void render(Menu const& menu) const {
        Serial.println("");
        for (int i = 0; i < menu.get_num_components(); ++i) {
            PlaylistMenuItem const* cp_m_comp = (PlaylistMenuItem*)menu.get_menu_component(i);
            cp_m_comp->render(*this);

            if (cp_m_comp->is_current())
            {
                //Serial.print("<<< ");
                
                animateLed(cp_m_comp->color, fill);
                
                timer.cancel(menuTimeoutTask);
                menuTimeoutTask = timer.in(5000, quitMenu);
            }
            //Serial.println("");
        }
    }

    void render_menu_item(MenuItem const& menu_item) const {
        //Serial.print(menu_item.get_name());
    }

    void render_back_menu_item(BackMenuItem const& menu_item) const {
        //Serial.print(menu_item.get_name());
    }

    void render_numeric_menu_item(NumericMenuItem const& menu_item) const {
        Serial.print(menu_item.get_name());
    }

    void render_menu(Menu const& menu) const {
        Serial.print(menu.get_name());
    }
};

MenuRenderer menuRenderer;
MenuSystem ms(menuRenderer);
PlaylistMenuItem* playingMenu = nullptr;
PlaylistMenuItem menuItems[]  = {	PlaylistMenuItem("BLUE"  , 0X07def3, 1, &onItemSelected),
                                    PlaylistMenuItem("PINK"  , 0Xff7df6, 2, &onItemSelected),//0Xff2f2f
                                    PlaylistMenuItem("GREEN" , 0X4ee64f, 3, &onItemSelected),
                                    PlaylistMenuItem("RED"   , 0Xff5757, 4, &onItemSelected),//PlaylistMenuItem("RED"   , 0Xff362c, 4, &onItemSelected),
                                    PlaylistMenuItem("PURPLE", 0X9836d7, 5, &onItemSelected),
                                    PlaylistMenuItem("YELLOW", 0Xffd100, 6, &onItemSelected)
						        };
#define ARRAYCOUNT(x)  (sizeof(x) / sizeof((x)[0]))

void onItemSelected(MenuComponent* p_menu_component) 
{
    PlaylistMenuItem* menu = (PlaylistMenuItem*)p_menu_component;

    if(playingMenu != menu)
    {
        if(playingMenu != nullptr)
            playingMenu->setPlaying(false);

        playingMenu = menu;
        playingMenu->setPlaying(true);
    }
    else
    {
        playingMenu->setPlaying(!playingMenu->isPlaying());
    }
}

bool quitMenu(void *argument /* optional argument given to in/at/every */) 
{
    Serial.println("quitMenu");

    for (int i = 0; i < ms.get_root_menu().get_num_components(); ++i) 
    {
        PlaylistMenuItem const* playingComp = (PlaylistMenuItem*) ms.get_root_menu().get_menu_component(i);
           
        if(playingComp->isPlaying())
        {
            animateLed(playingComp->color, rotate);

            if(!playingComp->is_current())
            {                
                ms.next(true);
                int walkIndex = 0;
                Serial.println("WALKING");

                while(ms.get_root_menu().get_current_component() != playingComp)
                {
                    Serial.print("WALKING step ");
                    Serial.println(ms.get_root_menu().get_current_component()->get_name());
                    ms.next(true);
                }

                Serial.println("WALKING done");
            }

            break;
        }
    }

    return false; // to repeat the action - false to stop
}

void beginMenu()
{
    for(int itemIndex = 0; itemIndex < ARRAYCOUNT(menuItems); ++itemIndex)
        ms.get_root_menu().add_item(&menuItems[itemIndex]);
        
    displayMenu();
}

void loopMenu()
{
    timer.tick();
}

void displayMenu()
{
    ms.display();
}

void triggerSelectMenu()
{   
    ms.select();
}

void triggerAltSelectMenu()
{
    nextAudioPlayer();
    playingMenu->setPlaying(true);
}

void triggerNextMenu()
{
    ms.next(true);
}

void triggerPreviousMenu()
{
     ms.prev(true);
}
