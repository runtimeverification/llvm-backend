#!/usr/bin/python

import yaml
import sys
from yaml import CLoader as Loader
from decimal import Decimal

blank_result  = {"count": 0, "shared_count": 0, "max_depth": 0, "max_choices": 0, "num_actions": 0, "sum_depth": 0, "sum_choices": 0}
leaf_result   = {"count": 0, "shared_count": 0, "max_depth": 1, "max_choices": 0, "num_actions": 0, "sum_depth": 0, "sum_choices": 0}
action_result = {"count": 1, "shared_count": 1, "max_depth": 1, "max_choices": 0, "num_actions": 1, "sum_depth": 1, "sum_choices": 0}

def count_nodes_shared(data):
    if isinstance(data, dict):
      if "result" in data:
        result = dict(data["result"])
        result["shared_count"] = 0
        return result
      if "next" in data:
        result = dict(count_nodes_shared(data["next"]))
        result["count"] += 1
        result["shared_count"] += 1
        result["max_depth"] += 1
        result["sum_depth"] += result["num_actions"]
        if "collection" in data:
          result["sum_choices"] += result["num_actions"]
          result["max_choices"] += 1
        data["result"] = result
        return result
      elif "specializations" in data:
        result = dict(blank_result)
        for case in data["specializations"]:
          case_result = count_nodes_shared(case[1])
          result["count"] += case_result["count"]
          result["shared_count"] += case_result["shared_count"]
          result["max_depth"] = max(case_result["max_depth"] + 1, result["max_depth"])
          result["max_choices"] = max(case_result["max_choices"], result["max_choices"])
          result["num_actions"] += case_result["num_actions"]
          result["sum_depth"] += case_result["sum_depth"] + case_result["num_actions"]
          result["sum_choices"] += case_result["sum_choices"]
        case_result = count_nodes_shared(data["default"])
        result["count"] += case_result["count"] + 1
        result["shared_count"] += case_result["shared_count"] + 1
        result["max_depth"] = max(case_result["max_depth"] + 1, result["max_depth"])
        result["max_choices"] = max(case_result["max_choices"], result["max_choices"])
        result["num_actions"] += case_result["num_actions"]
        result["sum_depth"] += case_result["sum_depth"] + case_result["num_actions"]
        result["sum_choices"] += case_result["sum_choices"]
        data["result"] = result
        return result
      elif "action" in data:
        data["result"] = action_result
        return action_result
    elif data == "fail":
      return leaf_result
    elif data == None:
      return blank_result
    else:
      print(type(data))
      raise AssertionError

with open(sys.argv[1], 'r') as stream:
  try:
    doc = yaml.load(stream, Loader=Loader)
    if (isinstance(doc, list)):
      result = count_nodes_shared(doc[0])
    else:
      result = count_nodes_shared(doc)
    
    print("Size: " + "{:,}".format(result["count"]))
    print("Shared size: " + "{:,}".format(result["shared_count"]))
    print("Max path length: " + str(result["max_depth"]))
    print("Average path length: " + str(Decimal(result["sum_depth"]) / Decimal(result["num_actions"])))
    print("Average number of choices: " + str(Decimal(result["sum_choices"]) / Decimal(result["num_actions"])))
    print("Max number of choices: " + str(result["max_choices"]))
  except yaml.YAMLError as exc:
    print(exc)
