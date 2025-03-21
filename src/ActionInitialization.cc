#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

ActionInitialization::ActionInitialization() {}
ActionInitialization::~ActionInitialization() {}

void ActionInitialization::Build() const {
    // Criar e registrar o gerador de partículas
    SetUserAction(new PrimaryGeneratorAction());

    // Criar e registrar RunAction
    RunAction* runAction = new RunAction();
    SetUserAction(runAction);

    // Criar o EventAction
    EventAction* eventAction = new EventAction();
    SetUserAction(eventAction);

    // SteppingAction recebe ponteiro para EventAction
    SetUserAction(new SteppingAction(eventAction));
}
