#include <visual/allegro_util.hpp>
#include "imgui_inc.hpp"

namespace visual {

    void initialize() {
        al_init();
        al_install_keyboard();
        al_install_mouse();
        al_init_primitives_addon();
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
    }

    void main_visual_loop(visual::EventReactor& user_events, ALLEGRO_DISPLAY* display) {
        auto system_events = visual::EventReactor();
        system_events.register_source(al_get_keyboard_event_source());
        system_events.register_source(al_get_display_event_source(display));
        while (true) {
            user_events.wait_and_react();

            if (system_events.empty()) {
                continue;
            }
            ALLEGRO_EVENT event;
            system_events.get(event);
            if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                break;
            }
        }
    }

    ALLEGRO_TIMER* Timer::init(double period) {
        return al_create_timer(period);   
    }

    void Timer::destroy(ALLEGRO_TIMER* al_timer) {
        al_destroy_timer(al_timer);
    }

    void Timer::start() {
        al_start_timer(al_pointer);
    }

    void Timer::stop() {
        al_stop_timer(al_pointer);
    }

    void Timer::change_rate(double new_rate) {
        al_set_timer_speed(al_pointer, new_rate);
    }

    double Timer::get_rate() const {
        return al_get_timer_speed(al_pointer);
    }

    ALLEGRO_EVENT_SOURCE* Timer::event_source() {
        return al_get_timer_event_source(al_pointer);
    }


    EventQueue::Raw* EventQueue::init() {
        return al_create_event_queue();
    }

    void EventQueue::destroy(EventQueue::Raw* raw) {
        al_destroy_event_queue(raw);
    }

    
    bool EventQueue::empty() const {
        return al_is_event_queue_empty(al_pointer);
    }

    void EventQueue::register_source(ALLEGRO_EVENT_SOURCE* source) {
        al_register_event_source(al_pointer, source);
    }

    void EventQueue::unregister_source(ALLEGRO_EVENT_SOURCE* source) {
        al_unregister_event_source(al_pointer, source);
    }
    
    void EventQueue::get(ALLEGRO_EVENT& event) {
        al_get_next_event(al_pointer, &event);
    }

    void EventQueue::peek(ALLEGRO_EVENT& event) {
        al_peek_next_event(al_pointer, &event);
    }

    void EventQueue::drop_one() {
        al_drop_next_event(al_pointer);
    }

    void EventQueue::drop_all() {
        al_flush_event_queue(al_pointer);
    }

    void EventQueue::wait() {
        al_wait_for_event(al_pointer, nullptr);
    }

    void EventQueue::wait(ALLEGRO_EVENT& event) {
        al_wait_for_event(al_pointer, &event);
    }

    void EventQueue::wait_for(ALLEGRO_EVENT& event, float seconds) {
        al_wait_for_event_timed(al_pointer, &event, seconds);
    }


    void EventReactor::add_reaction(const ALLEGRO_EVENT_SOURCE* source, Reaction reaction) {
        m_reactions[source].push_back(reaction);
    }

    void EventReactor::wait_and_react(ALLEGRO_EVENT& event) {
        EventQueue::wait(event);
        for (auto& reaction : m_reactions[event.any.source]) {
            reaction(event);
        }
    }

    void EventReactor::wait_and_react() {
        ALLEGRO_EVENT event;
        wait_and_react(event);
    }
}

