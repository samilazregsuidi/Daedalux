import datetime
import os
import shutil
from ChatGptClient import ChatGPTClient
from specification_generator import SpecificationGenerator
from training_data_creator import TrainingDataGenerator
from query_builder import QueryBuilder
from response_parser import ResponseParser
from spin_runner import Outcome, SpinRunner

class ModelExperiment:
    def __init__(self, model, difficulty):
        self.model = model
        self.difficulty = difficulty


def enhance_spec(model, surviving_mutants, specifications, chatGPTClient):
    """
    Refines the given model by alternately checking the model with SPIN and enhancing the specifications using ChatGPT based on the feedback from SPIN.
    
    Args:
    model (str): The file path to the model.
    surviving_mutants (list of str): The list of file paths to the surviving mutants.
    specifications (dict): The LTL specifications to enhance.
    chatGPTClient (ChatGPTClient): The ChatGPT client to use for querying.
    
    Returns:
    str: The refined model with enhanced specifications.
    """
    # Loop until the model has no compilation or verification errors and satisfies all LTL formulas
    
    # Keep track of the previously failed specifications
    previously_failed_specs = dict()
    previously_satisfied_specs = dict()
    
    number_of_iterations = 0
    number_of_iterations_before_human_intervention = 3
    
    while True:
        if number_of_iterations >= number_of_iterations_before_human_intervention:
            print(f"The model {model} could not be corrected in {number_of_iterations_before_human_intervention} iterations.")
            print(f"Please check the model {model} and the specifications manually to see if you can identify the issue")
            print("You can also check the console output to see which LTL formulas are not satisfied.")
            # Wait for the user to check the model and specifications
            print("Press Enter to continue after checking the model and specifications.")
            input()
            model_content = ''
            with open(model, 'r') as file:
                model_content = file.read()
            _, specifications = ResponseParser.extract_macros_and_ltl_properties(model_content)
            number_of_iterations = 0
            
        print(f"Checking the model: {model} with SPIN.")
        verdict, message, satisfied_formulas, failed_formula = SpinRunner.check_model(model, specifications)
        dict.update(previously_satisfied_specs, satisfied_formulas)
        
        if verdict == Outcome.COMPILATION_ERROR:
            print(f"Compilation error - trying to fix the model {model} by querying ChatGPT with the error message from SPIN.")
            # Handle compilation errors using ChatGPT
            query = QueryBuilder.fix_compilation_query_enhance(model, message)
            response = chatGPTClient.query_chatgpt(query)
            # Extract the updated specification from the response
            _, specifications = ResponseParser.extract_macros_and_ltl_properties(response)
            # Save the response to the model file by overwriting the existing content
            with open(model, 'w') as file:
                file.write(response)
        elif verdict == Outcome.VERIFICATION_ERROR:
            print(f"Verification error - trying to fix the model {model} by querying ChatGPT with the error message from SPIN.")
            # Keep track of the previously failed specifications
            dict.update(previously_failed_specs, failed_formula)
            # Handle verification errors
            counter_example_file = find_counter_example(model)
            with open(counter_example_file, 'r') as file:
                counter_example = file.read()
            query = QueryBuilder.fix_verification_query_enhance(model, message, counter_example, previously_satisfied_specs, previously_failed_specs)
            response = chatGPTClient.query_chatgpt(query)
            # Extract the updated specification from the response
            _, specifications = ResponseParser.extract_macros_and_ltl_properties(response)
            # Save the response to the model file by overwriting the existing content
            with open(model, 'w') as file:
                file.write(response)
        elif verdict == Outcome.SUCCESS:
            # If the model satisfies all LTL formulas
            print(f"Success! The model {model} satisfies all LTL formulas.")
            break
        else:
            print("Unexpected outcome:", verdict)
            break
        number_of_iterations += 1
        

        
def kill_mutants(model, mutants, specifications):
    """
    Eliminate all mutants that don't satisfy the LTL specifications.
    
    Args:
    model (str): The file path to the model.
    mutants (list of str): The list of file paths to the mutants.
    specifications (dict): The list of LTL specifications.
    
    Returns:
    list of str: The list of file paths to the surviving mutants.
    """
    surviving_mutants = []
    for mutant in mutants:
        result = SpinRunner.check_model(mutant, specifications)
        if result[0] == Outcome.SUCCESS:
            surviving_mutants.append(mutant)
        else:
            print(f"Mutant {mutant} eliminated. Reason: {result[0]}")
            # Remove the mutant file, the compiled files, the pan executable, and the trail files
            SpinRunner.remove_generated_files()
            
    return surviving_mutants

def distinguish_mutants(model, mutants, specifications, chatGPTClient, model_folder):
    """
    Distinguish the mutants that violate the LTL specifications from the ones that satisfy the LTL specifications.
    
    Args:
    model (str): The file path to the model.
    mutants (list of str): The list of file paths to the mutants.
    specifications (dict): The list of LTL specifications.
    
    Returns:
    """
    query_mutant = QueryBuilder.build_standard_query(model, mutants)
    response = chatGPTClient.query_chatgpt(query_mutant)
    macros, specifications = ResponseParser.parse_macros_and_specifications(response)
    updated_model = SpecificationGenerator.add_specification_to_model(model_folder, model, specifications, macros)
    SpecificationGenerator.refine_model(updated_model, specifications, chatGPTClient)
    with open(updated_model, 'r') as file:
        model_content = file.read()
    macros, specifications = ResponseParser.extract_macros_and_ltl_properties(model_content)
    
    kill_mutants(model, mutants, specifications)


def analyze_models(experiment_folder : str, model : str):
    """
    Analyzes the given original model by generating mutants and constructing queries for ChatGPT.
    
    Args:
    experiment_folder (str): The folder to store the experiment results.
    original_model (str): The file path to the original model.
    
    Returns:
    list of str: The list of file paths to the generated mutants.
    """
    
    # Ensure the original model exists
    if not os.path.exists(model):
        print(f"The original model file {model} does not exist.")
        return "The original model file does not exist."
    # Ensure the original model is a Promela file
    if not model.endswith('.pml'):
        return "The original model file should be a Promela file."
    
    # Get the file name of the model
    model_name = os.path.basename(model)
    print(f"Analyzing model: {model_name}")
    # Remove the file extension
    model_name = model_name.replace('.pml', '')
    
    # Construct a folder with the model name inside the experiment folder
    model_folder = os.path.join(experiment_folder, model_name)
    os.makedirs(model_folder)
    
    # Construct mutants
    mutants = generate_mutants(model)
    
    # Make a copy of the original model to the new folder
    shutil.copy(model, model_folder)
    
    file_model = os.path.join(model_folder, model_name + '.pml')
    # Generate traces for the original model
    number_of_traces = 5
    trace_length = 20
    trace_files = TraceGenerator.generate_trace(file_model, number_of_traces, trace_length)
    
    prompt_folder = os.path.join(model_folder, 'prompt')
    # Create a folder to store the prompt
    os.makedirs(prompt_folder)
    
    mutant_folder = os.path.join(model_folder, 'mutants')
    os.makedirs(mutant_folder)
    
    chatGPTClient = ChatGPTClient(api_key, prompt_folder)
    
    # Create Specification for the model
    updated_model = SpecificationGenerator.create_specification_model(file_model, model_folder, trace_files, chatGPTClient)
        
    # # Construct the query for ChatGPT
    # query = QueryBuilder.build_standard_query(file_model, mutants)
    
    # # Query ChatGPT with the constructed query
    # response = chatGPTClient.query_chatgpt(query)
    
    # macros, specifications = ResponseParser.parse_macros_and_specifications(response)
    
    # # Add the LTL specification to the original model
    # updated_model = add_specification_to_model(model_folder, model, specifications, macros)
        
    # refine_model(updated_model, specifications, chatGPTClient)
    
    # Extract the macros and LTL properties from the refined model
    with open(updated_model, 'r') as file:
        model_content = file.read()
    macros, specifications = ResponseParser.extract_macros_and_ltl_properties(model_content)
    
    mutants_with_spec = []
    # Move the mutants to the model folder
    for mutant in mutants:
        mutant_with_spec = add_specification_to_model(mutant_folder, mutant, specifications, macros)
        mutants_with_spec.append(mutant_with_spec)
        
    # Run SPIN on the updated model and use the response to enhance the specifications
    survivingMutants = SpinRunner.killMutants(mutants_with_spec, specifications)
    while survivingMutants != []:
        print("Surviving mutants:")
        for mutant in survivingMutants:
            print(f"Mutant {mutant} survived.")
            
        # Enhance the specifications using ChatGPT
        query = QueryBuilder.enhance_specification_query(updated_model, survivingMutants)
        response = chatGPTClient.query_chatgpt(query)
        macros, specifications = ResponseParser.parse_macros_and_specifications(response)
        
        print("Updated specifications:")
        for spec in specifications:
            print(f'{spec}: {specifications[spec]}')
            
        # Add the LTL specification to the original model
        print("Adding the LTL specification to the original model")
        temp_model = add_specification_to_model(model_folder, model, specifications, macros)
        refine_model(temp_model, specifications, chatGPTClient)
        # Extract the macros and LTL properties from the refined model
        with open(temp_model, 'r') as file:
            model_content = file.read()
            
        macros, specifications = ResponseParser.extract_macros_and_ltl_properties(model_content)
        
        updated_mutants = []
        for mutant in mutants_with_spec:
            mutant_with_spec = add_specification_to_model(mutant_folder, mutant, specifications, macros)
            updated_mutants.append(mutant_with_spec)
            
        print("Checking the surviving mutants with the updated model")
        survivingMutants = SpinRunner.killMutants(updated_mutants, specifications)
        
    print("All mutants eliminated. The model is refined with the following specifications:")
    for spec in specifications:
        print(f'{spec}: {specifications[spec]}')
        
    
def run_experiment():
    # Test the analyze_models function with some simple models
    minepump_model = ModelExperiment("../test_files/mutants/minepump/minepump.pml", 4)
    
    traffic_light_model = ModelExperiment("../test_files/mutants/trafficLight/trafficlight.pml", 1) 
    two_traffic_light_model = ModelExperiment("../test_files/mutants/trafficLight/two_trafficlight.pml", 2) 

    array_model = ModelExperiment("../test_files/mutants/array.pml", 2)
    dijkstra_model = ModelExperiment("../test_files/mutants/dijkstra/original.pml", 2)
    mutex_model = ModelExperiment("../test_files/mutants/mutex/mutex.pml", 2)
    three_process_model = ModelExperiment("../test_files/mutants/3Process/threeProcess.pml", 2)
    #peterson_model = "../test_files/mutants/peterson/original.pml"
    #flows_model = "../test_files/mutants/flows/flows.pml"
    
    # Create Experiment Folder
    date = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    folder_name = 'Experiment-' + date
    os.makedirs(folder_name)
    
    models = [traffic_light_model, two_traffic_light_model]
    for model in models:
        print(f"Analyzing model: {model.model}")
        ltl_formula = analyze_models(folder_name, model.model)

def generate_training_data():
    # Test the analyze_models function with some simple models
    minepump_model = ModelExperiment("../test_files/mutants/minepump/minepump.pml", 4)
    
    traffic_light_model = ModelExperiment("../test_files/mutants/trafficLight/trafficlight.pml", 1) 
    two_traffic_light_model = ModelExperiment("../test_files/mutants/trafficLight/two_trafficlight.pml", 2) 
    two_traffic_light_alt_model = ModelExperiment("../test_files/mutants/trafficLight/two_trafficlight_alt.pml", 2) 

    array_model = ModelExperiment("../test_files/mutants/array.pml", 2)
    dijkstra_model = ModelExperiment("../test_files/mutants/dijkstra/original.pml", 2)
    mutex_model = ModelExperiment("../test_files/mutants/mutex/mutex.pml", 2)
    three_process_model = ModelExperiment("../test_files/mutants/3Process/threeProcess.pml", 2)
    
    bakery_model = ModelExperiment("../test_files/mutants/bakery/bakery.pml", 2)
    
    dinning_philosophers_model = ModelExperiment("../test_files/mutants/diningPhilosophers/philosophers.pml", 2)
    
    peterson_model =  ModelExperiment("../test_files/mutants/peterson/original.pml", 2)
    queue_model =  ModelExperiment("../test_files/mutants/queue/queue.pml", 2)
    #flows_model = "../test_files/mutants/flows/flows.pml"
    
    leader_selection_model =  ModelExperiment("../test_files/mutants/leader/leader.pml", 2)
    train_model =  ModelExperiment("../test_files/mutants/train/train.pml", 2)
    
    elevator_model =  ModelExperiment("../test_files/mutants/elevator/elevator.pml", 2)
    
    adapro_model =  ModelExperiment("../test_files/mutants/adapro/adapro.pml", 2)

    models = [two_traffic_light_alt_model, train_model, peterson_model, minepump_model]
    
    training_generator = TrainingDataGenerator(models)
    
    # Create Experiment Folder
    date = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    folder_name = 'Training_data-' + date
    os.makedirs(folder_name)
    
    prompt_folder = os.path.join(folder_name, 'prompt')
    # Create a folder to store the prompt
    os.makedirs(prompt_folder)

    chatGPTClient = ChatGPTClient(api_key, prompt_folder)
    
    training_generator.create_training_data(folder_name, chatGPTClient, 100)
    

def main():
    generate_training_data()

if __name__ == "__main__":
    main()
