{
    gSystem->Load("libCore.so");
    gSystem->Load("libPhysics.so");
    gSystem->Load("libEG.so");

    gSystem->Load(gSystem->Getenv("RATROOT") + TString("/lib/libRATEvent.so"));
    gSystem->Load("validate.so");

    gInterpreter->Declare(R"(
        extern "C" void validate(int, bool, bool, int);
    )");
}
