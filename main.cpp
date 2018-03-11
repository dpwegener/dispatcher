#include <iostream>

struct Event {
    virtual ~Event() {}
    virtual int getId() const = 0;
};

struct Foo : Event {
    int getId() const override { return 1; }
    std::string name() const { return "Bob"; };
};

struct Bar : Event {
    int getId() const override { return 2; };
    std::string address() const { return "43"; };
};

struct Service {
    void accept(Foo const & aFoo) {
        std::cout << "You gave me a Foo called " << aFoo.name() << '\n';
    }

    void accept(Bar const & aBar) {
        std::cout << "You gave me a Bar from " << aBar.address() << '\n';
    }
};
template<typename T, typename S>
void acceptEvent(S & service, Event const & event) {
    service.accept(static_cast<T const &>(event));
}

struct Dispatcher {
    Dispatcher(Service & aService) : myService(aService) {};
    Service & myService;
    void accept(Event const & anEvent) {
        switch (anEvent.getId()) {
            case 1 : {
                acceptEvent<Foo>(myService, anEvent);
                break;
            }
            case 2 : {
                acceptEvent<Bar>(myService, anEvent);
                break;
            }
            default : {
                std::cout << "Unknown event: " << anEvent.getId() << '\n';
            }
        }
    }
};

int main(int, char**) {
    std::cout << "Hello, world!\n";
    Foo foo;
    Bar bar;
    std::cout << "Foo id: " << foo.getId() << " Bar id: " << bar.getId() << '\n';
    Service theService;
    theService.accept(foo);
    theService.accept(bar);
    Dispatcher dispatcher(theService);
    dispatcher.accept(bar);
    dispatcher.accept(foo);
}
