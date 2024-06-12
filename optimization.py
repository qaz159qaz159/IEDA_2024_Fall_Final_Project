


class Pin():
    def __init__(self, name, x, y):
        self.name = name
        self.x = x
        self.y = y

class Input():
    def __init__(self, name, x, y):
        self.name = name
        self.x = x
        self.y = y

class Output():
    def __init__(self, name, x, y):
        self.name = name
        self.x = x
        self.y = y

class Inputs():
    def __init__(self):
        self.inputs = []
    
    def add_input(self, input: Input):
        self.inputs.append(input)
    
    def get_input(self, name):
        for input in self.inputs:
            if input.name == name:
                return input
        return None

class Outputs():
    def __init__(self):
        self.outputs = []
    
    def add_output(self, output: Output):
        self.outputs.append(output)
    
    def get_output(self, name):
        for output in self.outputs:
            if output.name == name:
                return output
        return None

class FF():
    def __init__(self, name, bits, width, height, pinCount):
        self.name = name
        self.bits = bits
        self.width = width
        self.height = height
        self.pinCount = pinCount
        self.pins = []
    
    def add_pin(self, pin: Pin):
        self.pins.append(pin)
    
    def get_pin(self, name):
        for pin in self.pins:
            if pin.name == name:
                return pin
        return None

class FFs():
    def __init__(self):
        self.ffs = []
    
    def add_ff(self, ff: FF):
        self.ffs.append(ff)
    
    def get_ff(self, name):
        for ff in self.ffs:
            if ff.name == name:
                return ff
        return None

    def get_num_ff(self):
        return len(self.ffs)

class Gate():
    def __init__(self, name, width, height, pinCount):
        self.name = name
        self.width = width
        self.height = height
        self.pinCount = pinCount
        self.pins = []
    
    def add_pin(self, pin: Pin):
        self.pins.append(pin)
    
    def get_pin(self, name):
        for pin in self.pins:
            if pin.name == name:
                return pin
        return None

class Gates():
    def __init__(self):
        self.gates = []
    
    def add_gate(self, gate: Gate):
        self.gates.append(gate)
    
    def get_gate(self, name):
        for gate in self.gates:
            if gate.name == name:
                return gate
        return None

    def get_num_gates(self):
        return len(self.gates)

class Instance():
    def __init__(self, instName, libCellName, x, y):
        self.instName = instName
        self.libCellName = libCellName
        self.x = x
        self.y = y
    
class Instances():
    def __init__(self):
        self.instances = []
    
    def add_instance(self, instance: Instance):
        self.instances.append(instance)
    
    def get_instance(self, instName):
        for instance in self.instances:
            if instance.instName == instName:
                return instance
        return None

    def get_num_instances(self):
        return len(self.instances)

class NetPin():
    def __init__(self, instName, pinName):
        self.instName = instName
        self.pinName = pinName
    
class Net():
    def __init__(self, netName, pinCount):
        self.netName = netName
        self.pinCount = pinCount
        self.netPins = []
    
    def add_net_pin(self, netPin: NetPin):
        self.netPins.append(netPin)
    
    def get_net_pin(self, instName, pinName):
        for netPin in self.netPins:
            if netPin.instName == instName and netPin.pinName == pinName:
                return netPin
        return None

class Nets():
    def __init__(self):
        self.nets = []
    
    def add_net(self, net: Net):
        self.nets.append(net)
    
    def get_net(self, netName):
        for net in self.nets:
            if net.netName == netName:
                return net
        return None

    def get_num_nets(self):
        return len(self.nets)

class PlacementRow():
    def __init__(self, startX, startY, numSites, siteWidth, siteHeight):
        self.startX = startX
        self.startY = startY
        self.numSites = numSites
        self.siteWidth = siteWidth
        self.siteHeight = siteHeight

class PlacementRows():
    def __init__(self):
        self.placementRows = []
    
    def add_placement_row(self, placementRow: PlacementRow):
        self.placementRows.append(placementRow)
    
    def get_placement_row(self, y):
        for placementRow in self.placementRows:
            if placementRow.startY == y:
                return placementRow
        return None

class QPinDelay():
    def __init__(self, pinName, delay):
        self.pinName = pinName
        self.delay = delay

class QPinDelays():
    def __init__(self):
        self.qPinDelays = []
    
    def add_q_pin_delay(self, qPinDelay: QPinDelay):
        self.qPinDelays.append(qPinDelay)
    
    def get_q_pin_delay(self, pinName):
        for qPinDelay in self.qPinDelays:
            if qPinDelay.pinName == pinName:
                return qPinDelay
        return None

class TimeSlack():
    def __init__(self, instName, pinName, slack):
        self.instName = instName
        self.pinName = pinName
        self.slack = slack

class TimeSlacks():
    def __init__(self):
        self.timeSlacks = []
    
    def add_time_slack(self, timeSlack: TimeSlack):
        self.timeSlacks.append(timeSlack)
    
    def get_time_slack(self, instName, pinName):
        for timeSlack in self.timeSlacks:
            if timeSlack.instName == instName and timeSlack.pinName == pinName:
                return timeSlack
        return None

class GatePower():
    def __init__(self, instName, power):
        self.instName = instName
        self.power = power

class GatePowers():
    def __init__(self):
        self.gatePowers = []
    
    def add_gate_power(self, gatePower: GatePower):
        self.gatePowers.append(gatePower)
    
    def get_gate_power(self, instName, pinName):
        for gatePower in self.gatePowers:
            if gatePower.instName == instName and gatePower.pinName == pinName:
                return gatePower
        return None

class DisplacementDelay():
    def __init__(self, coef):
        self.coef = coef

class Bin():
    def __init__(self, width, height, maxUtil):
        self.width = width
        self.height = height
        self.maxUtil = maxUtil

class Die():
    def __init__(self, lowerLeftX, lowerLeftY, upperRightX, upperRightY):
        self.lowerLeftX = lowerLeftX
        self.lowerLeftY = lowerLeftY
        self.upperRightX = upperRightX
        self.upperRightY = upperRightY

class Mapping():
    def __init__(self):
        self.mapping = []

class FFCellLib():
    def __init__(self):
        self.ffCellLib = {1: [], 2: [], 4: []}
    
    def build_ff_cell_lib(self, ffs):
        for ff in ffs:
            self.ffCellLib[ff.bits].append(ff)
    
    def __repr__(self):
        return str([len(self.ffCellLib[1]), len(self.ffCellLib[2]), len(self.ffCellLib[4])])
            

class Optimization():
    def __init__(self):
        self.inputs = Inputs()
        self.outputs = Outputs()
        self.ffs = FFs()
        self.gates = Gates()
        self.instances = Instances()
        self.nets = Nets()
        self.placementRows = PlacementRows()
        self.qPinDelays = QPinDelays()
        self.timeSlacks = TimeSlacks()
        self.gatePowers = GatePowers()
        self.displacementDelay = DisplacementDelay(0)
        self.bin = Bin(0, 0, 0)
        self.die = Die(0, 0, 0, 0)
        # Params
        self.alpha = 0
        self.beta = 0
        self.gamma = 0
        self.lambda_ = 0
        
        # New
        self.ffCellLib = FFCellLib()
        
        
    def read_input(self, filename):
        with open(filename, 'r') as f:
            lines = f.readlines()
        
        i = 0
        while i < len(lines):
            line = lines[i].strip()
            if line.startswith("Alpha"):
                self.alpha = float(line.split()[1])
            elif line.startswith("Beta"):
                self.beta = float(line.split()[1])
            elif line.startswith("Gamma"):
                self.gamma = float(line.split()[1])
            elif line.startswith("Lambda"):
                self.lambda_ = float(line.split()[1])
            elif line.startswith("DieSize"):
                _, llx, lly, urx, ury = line.split()
                self.die = Die(int(llx), int(lly), int(urx), int(ury))
            elif line.startswith("NumInput"):
                num_inputs = int(line.split()[1])
                for _ in range(num_inputs):
                    i += 1
                    inp = lines[i].strip().split()
                    input_obj = Input(inp[1], int(inp[2]), int(inp[3]))
                    self.inputs.add_input(input_obj)
            elif line.startswith("NumOutput"):
                num_outputs = int(line.split()[1])
                for _ in range(num_outputs):
                    i += 1
                    outp = lines[i].strip().split()
                    output_obj = Output(outp[1], int(outp[2]), int(outp[3]))
                    self.outputs.add_output(output_obj)
            elif line.startswith("FlipFlop"):
                _, bits, name, width, height, pin_count = line.split()
                ff = FF(name, int(bits), int(width), int(height), int(pin_count))
                for _ in range(int(pin_count)):
                    i += 1
                    pin_data = lines[i].strip().split()
                    pin = Pin(pin_data[1], int(pin_data[2]), int(pin_data[3]))
                    ff.add_pin(pin)
                self.ffs.add_ff(ff)
            elif line.startswith("Gate") and not line.startswith("GatePower"):
                _, name, width, height, pin_count = line.split()
                gate = Gate(name, int(width), int(height), int(pin_count))
                for _ in range(int(pin_count)):
                    i += 1
                    pin_data = lines[i].strip().split()
                    pin = Pin(pin_data[1], int(pin_data[2]), int(pin_data[3]))
                    gate.add_pin(pin)
                self.gates.add_gate(gate)
            elif line.startswith("NumInstances"):
                num_instances = int(line.split()[1])
                for _ in range(num_instances):
                    i += 1
                    inst_data = lines[i].strip().split()
                    instance = Instance(inst_data[1], inst_data[2], int(inst_data[3]), int(inst_data[4]))
                    self.instances.add_instance(instance)
            elif line.startswith("NumNets"):
                num_nets = int(line.split()[1])
                for _ in range(num_nets):
                    i += 1
                    net_data = lines[i].strip().split()
                    net = Net(net_data[1], int(net_data[2]))
                    for _ in range(int(net_data[2])):
                        i += 1
                        if '/' in lines[i]:
                            pin_data = lines[i].strip().split()
                            pin_data = pin_data[1].strip().split('/')
                            net_pin = NetPin(pin_data[0], pin_data[1])
                        else:
                            pin_data = lines[i].strip().split()
                            net_pin = NetPin(None, pin_data[1])
                        net.add_net_pin(net_pin)
                    self.nets.add_net(net)
            elif line.startswith("BinWidth"):
                bin_width = int(line.split()[1])
                i += 1
                bin_height = int(lines[i].strip().split()[1])
                i += 1
                bin_max_util = float(lines[i].strip().split()[1])
                self.bin = Bin(bin_width, bin_height, bin_max_util)
            elif line.startswith("PlacementRows"):
                placement_row_data = line.split()
                row = PlacementRow(int(placement_row_data[1]), int(placement_row_data[2]), int(placement_row_data[3]), int(placement_row_data[4]), int(placement_row_data[5]))
                self.placementRows.add_placement_row(row)
            elif line.startswith("DisplacementDelay"):
                self.displacementDelay.coef = float(line.split()[1])
            elif line.startswith("QpinDelay"):
                qpin_data = line.split()
                qpin = QPinDelay(qpin_data[1], float(qpin_data[2]))
                self.qPinDelays.add_q_pin_delay(qpin)
            elif line.startswith("TimingSlack"):
                slack_data = line.split()
                slack = TimeSlack(slack_data[1], slack_data[2], float(slack_data[3]))
                self.timeSlacks.add_time_slack(slack)
            elif line.startswith("GatePower"):
                power_data = line.split()
                power = GatePower(power_data[1], float(power_data[2]))
                self.gatePowers.add_gate_power(power)
            i += 1
        print("Read input successfully")
        
    def optimization(self):
        self.read_input("testcase1.txt")
        
        self.ffCellLib.build_ff_cell_lib(self.ffs.ffs)
        print(self.find_CLK_FF_nets_with_more_than_2_pins())

    def find_CLK_FF_nets_with_more_than_2_pins(self):
        CLK_nets = []
        for net in self.nets.nets:
            if net.pinCount > 2:
                for netPin in net.netPins:
                    if netPin.pinName == "CLK":
                        CLK_nets.append(net)
                        break
        instance_names = []
        return CLK_nets
    
    def sort_nets_by_pin_count(self, net_list):
        net_list.sort(key=lambda x: x.pinCount)
        return net_list
    
    def get_ff_instances(self):
        ff_instances = []
        for instance in self.instances.instances:
            if "FF" in instance.libCellName:
                ff_instances.append(instance)
        return ff_instances
    
    def sort_instances(self):
        # Sort instances by x, y from left to right, bottom to top
        self.instances.instances.sort(key=lambda x: (x.y, x.x))
    
    def check_inst_status(self, instance: Instance):
        if "FF" in instance.libCellName:
            inst = self.ffs.get_ff(instance.libCellName)
        elif "Gate" in instance.libCellName:
            inst = self.gates.get_gate(instance.libCellName)
        # 1. In the die region
        if instance.x < self.die.lowerLeftX or instance.x + inst.width > self.die.upperRightX or instance.y < self.die.lowerLeftY or instance.y + inst.height > self.die.upperRightY:
            return False
        # 2. No overlap with other instances
        for other_instance in self.instances.instances:
            if other_instance.instName == instance.instName:
                continue
            if (instance.x < other_instance.x + inst.width and instance.y < other_instance.y + inst.height) or (instance.x + inst.width > other_instance.x and instance.y + inst.height > other_instance.y):
                return False
        # 3. TODO: A list of banking and debanking mapping needs to be provided
        pass
    
        # 4. TODO: Nets connected to the flip-flops must remain functionally equivalent to the data input. The result should not leave any open or short net. 
        pass
    
    def bank_ffs(self, ff1: Instance, ff2: Instance, banked_ff: Instance):
        # The two flip-flops' CLK pins must from the same Inst
        
        # Position use the lower left corner of the two flip-flops
        banked_ff.x = min(ff1.x, ff2.x)
        banked_ff.y = min(ff1.y, ff2.y)
        
        # TODO: choose a banked FF from the list of banked FFs
        banked_ff.libCellName = ""
        banked_ff.instName = ""
        
        
        

def print_ff_status(ffs):
    num_single_bit_ff = 0
    num_multi_bit_ff = 0
    for ff in ffs.ffs:
        if ff.bits == 1:
            num_single_bit_ff += 1
        else:
            num_multi_bit_ff += 1
    print("Single bit FFs: ", num_single_bit_ff) # 31 single bit FFs
    print("Multi bit FFs: ", num_multi_bit_ff) # 6 2-bits FFs, 10 4-bits FFs

if __name__ == "__main__":
    opt = Optimization()
    opt.optimization()