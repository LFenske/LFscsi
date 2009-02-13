/*
 * Copyright (C) 2008  Larry Fenske
 * 
 * This file is part of LFscsi.
 * 
 * LFscsi is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * LFscsi is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with LFscsi.  If not, see <http://www.gnu.org/licenses/>.
 */
#define __PrintRequestSenseSub_c

#include "common.h"
#include <stdio.h>    /* for printf, NULL */
#include <stdlib.h>   /* for malloc */
#include <string.h>   /* for memcpy */
#include <ctype.h>    /* for isprint */


const char *keyname[] = {
  "NO SENSE",          /* 0 */
  "RECOVERED ERROR",   /* 1 */
  "NOT READY",         /* 2 */
  "MEDIUM ERROR",      /* 3 */
  "HARDWARE ERROR",    /* 4 */
  "ILLEGAL REQUEST",   /* 5 */
  "UNIT ATTENTION",    /* 6 */
  "DATA PROTECT",      /* 7 */
  "BLANK CHECK",       /* 8 */
  "VENDOR-SPECIFIC",   /* 9 */
  "COPY ABORTED",      /* A */
  "ABORTED COMMAND",   /* B */
  "EQUAL",             /* C */
  "VOLUME OVERFLOW",   /* D */
  "MISCOMPARE",        /* E */
  "RESERVED",          /* F */
};


tabletype asc_q[] = {
/* specific to media changer devices*/
  {0x0200, "NO SEEK COMPLETE"           },
  {0x0600, "NO REFERENCE POSITION FOUND"},

/* specific to sequential access devices*/
  {0x0001, "FILEMARK DETECTED"                                },
  {0x0002, "END-OF-PARTITION/MEDIUM DETECTED"                 },
  {0x0003, "SETMARK DETECTED"                                 },
  {0x0004, "BEGINNING-OF-PARTITION/MEDIUM DETECTED"           },
  {0x0005, "END-OF-DATA DETECTED"                             },
  {0x0018, "ERASE OPERATION IN PROGRESS"                      },
  {0x0019, "LOCATE OPERATION IN PROGRESS"                     },
  {0x001A, "REWIND OPERATION IN PROGRESS"                     },
  {0x001B, "SET CAPACITY OPERATION IN PROGRESS"               },
  {0x001C, "VERIFY OPERATION IN PROGRESS"                     },
  {0x0300, "PERIPHERAL DEVICE WRITE FAULT"                    },
  {0x0301, "NO WRITE CURRENT"                                 },
  {0x0302, "EXCESSIVE WRITE ERRORS"                           },
  {0x0404, "LOGICAL UNIT NOT READY, FORMAT IN PROGRESS"       },
  {0x0804, "UNREACHABLE COPY TARGET"                          },
  {0x0900, "TRACK FOLLOWING ERROR"                            },
  {0x0904, "HEAD SELECT FAULT"                                },
  {0x0C00, "WRITE ERROR"                                      },
  {0x0C04, "COMPRESSION CHECK MISCOMPARE ERROR"               },
  {0x0C05, "DATA EXPANSION OCCURRED DURING COMPRESSION"       },
  {0x0C06, "BLOCK NOT COMPRESSIBLE"                           },
  {0x0D00, "ERROR DETECTED BY THIRD PARTY TEMPORARY INITIATOR"},
  {0x0D01, "THIRD PARTY DEVICE FAILURE"                       },
  {0x0D02, "COPY TARGET DEVICE NOT REACHABLE"                 },
  {0x0D03, "INCORRECT COPY TARGET DEVICE TYPE"                },
  {0x0D04, "COPY TARGET DEVICE DATA UNDERRUN"                 },
  {0x0D05, "COPY TARGET DEVICE DATA OVERRUN"                  },
  {0x1100, "UNRECOVERED READ ERROR"                           },
  {0x1101, "READ RETRIES EXHAUSTED"                           },
  {0x1102, "ERROR TOO LONG TO CORRECT"                        },
  {0x1103, "MULTIPLE READ ERRORS"                             },
  {0x1108, "INCOMPLETE BLOCK READ"                            },
  {0x1109, "NO GAP FOUND"                                     },
  {0x110A, "MISCORRECTED ERROR"                               },
  {0x110D, "DE COMPRESSION CRC ERROR"                         },
  {0x110E, "CANNOT DECOMPRESS USING DECLARED ALGORITHM"       },
  {0x1400, "RECORDED ENTITY NOT FOUND"                        },
  {0x1401, "RECORD NOT FOUND"                                 },
  {0x1402, "FILEMARK OR SETMARK NOT FOUND"                    },
  {0x1403, "END-OF-DATA NOT FOUND"                            },
  {0x1404, "BLOCK SEQUENCE ERROR"                             },
  {0x1405, "RECORD NOT FOUND - RECOMMEND REASSIGNMENT"        },
  {0x1406, "RECORD NOT FOUND - DATA AUTO REALLOCATED"         },
  {0x1407, "LOCATE OPERATION FAILURE"                         },
  {0x1502, "POSITIONING ERROR DETECTED BY READ OF MEDIUM"     },
  {0x1700, "RECOVERED DATA WITH NO ERROR CORRECTION APPLIED"  },
  {0x1701, "RECOVERED DATA WITH RETRIES"                      },
  {0x1702, "RECOVERED DATA WITH POSITIVE HEAD OFFSET"         },
  {0x1703, "RECOVERED DATA WITH NEGATIVE HEAD OFFSET"         },
  {0x1800, "K RECOVERED DATA WITH ERROR CORRECTION APPLIED"   },
  {0x1D00, "MISCOMPARE DURING VERIFY OPERATION"               },
  {0x2004, "ILLEGAL COMMAND WHILE IN WRITE CAPABLE STATE"     },
  {0x2005, "Obsolete"                                         },
  {0x2006, "ILLEGAL COMMAND WHILE IN EXPLICIT ADDRESS MODE"   },
  {0x2007, "ILLEGAL COMMAND WHILE IN IMPLICIT ADDRESS MODE"   },
  {0x2606, "TOO MANY TARGET DESCRIPTORS"                      },
  {0x2607, "UNSUPPORTED TARGET DESCRIPTOR TYPE CODE"          },
  {0x2608, "TOO MANY SEGMENT DESCRIPTORS"                     },
  {0x2609, "UNSUPPORTED SEGMENT DESCRIPTOR TYPE CODE"         },
  {0x260A, "UNEXPECTED INEXACT SEGMENT"                       },
  {0x260B, "INLINE DATA LENGTH EXCEEDED"                      },
  {0x260C, "INVALID OPERATION FOR COPY SOURCE OR DESTINATION" },
  {0x260D, "COPY SEGMENT GRANULARITY VIOLATION"               },
  {0x2700, "WRITE PROTECTED"                                  },
  {0x2701, "HARDWARE WRITE PROTECTED"                         },
  {0x2702, "LOGICAL UNIT SOFTWARE WRITE PROTECTED"            },
  {0x2703, "ASSOCIATED WRITE PROTECT"                         },
  {0x2704, "PERSISTENT WRITE PROTECT"                         },
  {0x2705, "PERMANENT WRITE PROTECT"                          },
  {0x2B00, "COPY CANNOT EXECUTE SINCE HOST CANNOT DISCONNECT" },
  {0x2D00, "OVERWRITE ERROR ON UPDATE IN PLACE"               },
  {0x3001, "CANNOT READ MEDIUM - UNKNOWN FORMAT"              },
  {0x3002, "CANNOT READ MEDIUM - INCOMPATIBLE FORMAT"         },
  {0x3003, "CLEANING CARTRIDGE INSTALLED"                     },
  {0x3004, "CANNOT WRITE MEDIUM - UNKNOWN FORMAT"             },
  {0x3005, "CANNOT WRITE MEDIUM - INCOMPATIBLE FORMAT"        },
  {0x3006, "CANNOT FORMAT MEDIUM - INCOMPATIBLE MEDIUM"       },
  {0x3100, "MEDIUM FORMAT CORRUPTED"                          },
  {0x3300, "TAPE LENGTH ERROR"                                },
  {0x3B00, "SEQUENTIAL POSITIONING ERROR"                     },
  {0x3B01, "TAPE POSITION ERROR AT BEGINNING-OF-MEDIUM"       },
  {0x3B02, "TAPE POSITION ERROR AT END-OF-MEDIUM"             },
  {0x3B08, "REPOSITION ERROR"                                 },
  {0x3B0C, "POSITION PAST BEGINNING OF MEDIUM"                },
  {0x5000, "WRITE APPEND ERROR"                               },
  {0x5001, "WRITE APPEND POSITION ERROR"                      },
  {0x5002, "POSITION ERROR RELATED TO TIMING"                 },
  {0x5100, "ERASE FAILURE"                                    },
  {0x5200, "CARTRIDGE FAULT"                                  },
  {0x5301, "UNLOAD TAPE FAILURE"                              },
  {0x5A02, "OPERATOR SELECTED WRITE PROTECT"                  },
  {0x5A03, "OPERATOR SELECTED WRITE PERMIT"                   },
  {0x5E00, "LOW POWER CONDITION ON"                           },
  {0x5E01, "IDLE CONDITION ACTIVATED BY TIMER"                },
  {0x5E02, "STANDBY CONDITION ACTIVATED BY TIMER"             },
  {0x5E03, "IDLE CONDITION ACTIVATED BY COMMAND"              },
  {0x5E04, "STANDBY CONDITION ACTIVATED BY COMMAND"           },
  {0x7100, "DECOMPRESSION EXCEPTION LONG ALGORITHM ID"        },

/* common to media changers and sequential access devices */
  {0x0000, "NO ADDITIONAL SENSE INFORMATION"                                },
  {0x0006, "I/O PROCESS TERMINATED"                                         },
  {0x0016, "OPERATION IN PROGRESS"                                          },
  {0x0017, "CLEANING REQUESTED"                                             },
  {0x0400, "LOGICAL UNIT NOT READY, CAUSE NOT REPORTABLE"                   },
  {0x0401, "LOGICAL UNIT IS IN PROCESS OF BECOMING READY"                   },
  {0x0402, "LOGICAL UNIT NOT READY, INITIALIZING COMMAND REQUIRED"          },
  {0x0403, "LOGICAL UNIT NOT READY, MANUAL INTERVENTION REQUIRED"           },
  {0x0405, "LOGICAL UNIT NOT READY, REBUILD IN PROGRESS"                    },
  {0x0406, "LOGICAL UNIT NOT READY, RECALCULATION IN PROGRESS"              },
  {0x0407, "LOGICAL UNIT NOT READY, OPERATION IN PROGRESS"                  },
  {0x0409, "LOGICAL UNIT NOT READY, SELF-TEST IN PROGRESS"                  },
  {0x040A, "LOGICAL UNIT NOT ACCESSIBLE, ASYMMETRIC ACCESS STATE TRANSITION"},
  {0x040B, "LOGICAL UNIT NOT ACCESSIBLE, TARGET PORT IN STANDBY STATE"      },
  {0x040C, "LOGICAL UNIT NOT ACCESSIBLE, TARGET PORT IN UNAVAILABLE STATE"  },
  {0x0410, "LOGICAL UNIT NOT READY, AUXILIARY MEMORY NOT ACCESSIBLE"        },
  {0x0500, "LOGICAL UNIT DOES NOT RESPOND TO SELECTION"                     },
  {0x0700, "MULTIPLE PERIPHERAL DEVICES SELECTED"                           },
  {0x0800, "LOGICAL UNIT COMMUNICATION FAILURE"                             },
  {0x0801, "LOGICAL UNIT COMMUNICATION TIME-OUT"                            },
  {0x0802, "LOGICAL UNIT COMMUNICATION PARITY ERROR"                        },
  {0x0803, "LOGICAL UNIT COMMUNICATION CRC ERROR (ULTRA-DMA/32)"            },
  {0x0A00, "ERROR LOG OVERFLOW"                                             },
  {0x0B00, "WARNING"                                                        },
  {0x0B01, "WARNING - SPECIFIED TEMPERATURE EXCEEDED"                       },
  {0x0B02, "WARNING - ENCLOSURE DEGRADED"                                   },
  {0x0C0B, "AUXILIARY MEMORY WRITE ERROR"                                   },
  {0x0C0C, "WRITE ERROR - UNEXPECTED UNSOLICITED DATA"                      },
  {0x0C0D, "WRITE ERROR - NOT ENOUGH UNSOLICITED DATA"                      },
  {0x0E00, "INVALID INFORMATION UNIT"                                       },
  {0x0E01, "INFORMATION UNIT TOO SHORT"                                     },
  {0x0E02, "INFORMATION UNIT TOO LONG"                                      },
  {0x1112, "AUXILIARY MEMORY READ ERROR"                                    },
  {0x1113, "READ ERROR - FAILED RETRANSMISSION REQUEST"                     },
  {0x1500, "RANDOM POSITIONING ERROR"                                       },
  {0x1501, "MECHANICAL POSITIONING ERROR"                                   },
  {0x1A00, "PARAMETER LIST LENGTH ERROR"                                    },
  {0x1B00, "SYNCHRONOUS DATA TRANSFER ERROR"                                },
  {0x2000, "INVALID COMMAND OPERATION CODE"                                 },
  {0x2001, "ACCESS DENIED - INITIATOR PENDING ENROLLED"                     },
  {0x2002, "ACCESS DENIED - NO ACCESS RIGHTS"                               },
  {0x2003, "ACCESS DENIED - INVALID MGMT ID KEY"                            },
  {0x2008, "ACCESS DENIED - ENROLLMENT CONFLICT"                            },
  {0x2009, "ACCESS DENIED - INVALID LU IDENTIFIER"                          },
  {0x200A, "ACCESS DENIED - INVALID PROXY TOKEN"                            },
  {0x200B, "ACCESS DENIED - ACL LUN CONFLICT"                               },
  {0x2100, "LOGICAL BLOCK ADDRESS OUT OF RANGE"                             },
  {0x2101, "INVALID ELEMENT ADDRESS"                                        },
  {0x2400, "INVALID FIELD IN CDB"                                           },
  {0x2401, "CDB DECRYPTION ERROR"                                           },
  {0x2500, "LOGICAL UNIT NOT SUPPORTED"                                     },
  {0x2600, "INVALID FIELD IN PARAMETER LIST"                                },
  {0x2601, "PARAMETER NOT SUPPORTED"                                        },
  {0x2602, "PARAMETER VALUE INVALID"                                        },
  {0x2603, "THRESHOLD PARAMETERS NOT SUPPORTED"                             },
  {0x2604, "INVALID RELEASE OF PERSISTENT RESERVATION"                      },
  {0x2605, "DATA DECRYPTION ERROR"                                          },
  {0x2800, "NOT READY TO READY CHANGE, MEDIUM MAY HAVE CHANGED"             },
  {0x2801, "IMPORT OR EXPORT ELEMENT ACCESSED"                              },
  {0x2900, "POWER ON, RESET, OR BUS DEVICE RESET OCCURRED"                  },
  {0x2901, "POWER ON OCCURRED"                                              },
  {0x2902, "SCSI BUS RESET OCCURRED"                                        },
  {0x2903, "BUS DEVICE RESET FUNCTION OCCURRED"                             },
  {0x2904, "DEVICE INTERNAL RESET"                                          },
  {0x2905, "TRANSCEIVER MODE CHANGED TO SINGLE-ENDED"                       },
  {0x2906, "TRANSCEIVER MODE CHANGED TO LVD"                                },
  {0x2907, "I_T NEXUS LOSS OCCURRED"                                        },
  {0x2A00, "PARAMETERS CHANGED"                                             },
  {0x2A01, "MODE PARAMETERS CHANGED"                                        },
  {0x2A02, "LOG PARAMETERS CHANGED"                                         },
  {0x2A03, "RESERVATIONS PREEMPTED"                                         },
  {0x2A04, "RESERVATIONS RELEASED"                                          },
  {0x2A05, "REGISTRATIONS PREEMPTED"                                        },
  {0x2A06, "ASYMMETRIC ACCESS STATE CHANGED"                                },
  {0x2A07, "IMPLICIT ASYMMETRIC ACCESS STATE TRANSITION FAILED"             },
  {0x2C00, "COMMAND SEQUENCE ERROR"                                         },
  {0x2C07, "PREVIOUS BUSY STATUS"                                           },
  {0x2C08, "PREVIOUS TASK SET FULL STATUS"                                  },
  {0x2C09, "PREVIOUS RESERVATION CONFLICT STATUS"                           },
  {0x2F00, "COMMANDS CLEARED BY ANOTHER INITIATOR"                          },
  {0x3000, "INCOMPATIBLE MEDIUM INSTALLED"                                  },
  {0x3007, "CLEANING FAILURE"                                               },
  {0x3400, "ENCLOSURE FAILURE"                                              },
  {0x3500, "ENCLOSURE SERVICES FAILURE"                                     },
  {0x3501, "UNSUPPORTED ENCLOSURE FUNCTION"                                 },
  {0x3502, "ENCLOSURE SERVICES UNAVAILABLE"                                 },
  {0x3503, "ENCLOSURE SERVICES TRANSFER FAILURE"                            },
  {0x3504, "ENCLOSURE SERVICES TRANSFER REFUSED"                            },
  {0x3700, "ROUNDED PARAMETER"                                              },
  {0x3900, "SAVING PARAMETERS NOT SUPPORTED"                                },
  {0x3A00, "MEDIUM NOT PRESENT"                                             },
  {0x3A01, "MEDIUM NOT PRESENT - TRAY CLOSED"                               },
  {0x3A02, "MEDIUM NOT PRESENT - TRAY OPEN"                                 },
  {0x3A03, "MEDIUM NOT PRESENT - LOADABLE"                                  },
  {0x3A04, "MEDIUM NOT PRESENT - MEDIUM AUXILIARY MEMORY ACCESSIBLE"        },
  {0x3B0D, "MEDIUM DESTINATION ELEMENT FULL"                                },
  {0x3B0E, "MEDIUM SOURCE ELEMENT EMPTY"                                    },
  {0x3B11, "MEDIUM MAGAZINE NOT ACCESSIBLE"                                 },
  {0x3B12, "MEDIUM MAGAZINE REMOVED"                                        },
  {0x3B13, "MEDIUM MAGAZINE INSERTED"                                       },
  {0x3B14, "MEDIUM MAGAZINE LOCKED"                                         },
  {0x3B15, "MEDIUM MAGAZINE UNLOCKED"                                       },
  {0x3D00, "INVALID BITS IN IDENTIFY MESSAGE"                               },
  {0x3E00, "LOGICAL UNIT HAS NOT SELF CONFIGURED YET"                       },
  {0x3E01, "LOGICAL UNIT FAILURE"                                           },
  {0x3E02, "TIMEOUT ON LOGICAL UNIT"                                        },
  {0x3E03, "LOGICAL UNIT FAILED SELF-TEST"                                  },
  {0x3E04, "LOGICAL UNIT UNABLE TO UPDATE SELF-TEST LOG"                    },
  {0x3F00, "TARGET OPERATING CONDITIONS HAVE CHANGED"                       },
  {0x3F01, "MICROCODE HAS BEEN CHANGED"                                     },
  {0x3F02, "CHANGED OPERATING DEFINITION"                                   },
  {0x3F03, "INQUIRY DATA HAS CHANGED"                                       },
  {0x3F04, "COMPONENT DEVICE ATTACHED"                                      },
  {0x3F05, "DEVICE IDENTIFIER CHANGED"                                      },
  {0x3F06, "REDUNDANCY GROUP CREATED OR MODIFIED"                           },
  {0x3F07, "REDUNDANCY GROUP DELETED"                                       },
  {0x3F08, "SPARE CREATED OR MODIFIED"                                      },
  {0x3F09, "SPARE DELETED"                                                  },
  {0x3F0A, "VOLUME SET CREATED OR MODIFIED"                                 },
  {0x3F0B, "VOLUME SET DELETED"                                             },
  {0x3F0C, "VOLUME SET DEASSIGNED"                                          },
  {0x3F0D, "VOLUME SET REASSIGNED"                                          },
  {0x3F0E, "REPORTED LUNS DATA HAS CHANGED"                                 },
  {0x3F0F, "ECHO BUFFER OVERWRITTEN"                                        },
  {0x3F10, "MEDIUM LOADABLE"                                                },
  {0x3F11, "MEDIUM AUXILIARY MEMORY ACCESSIBLE"                             },
  {0x4000, "DIAGNOSTIC FAILURE ON COMPONENT NN (80H-FFH)"                   },
  {0x4300, "MESSAGE ERROR"                                                  },
  {0x4400, "INTERNAL TARGET FAILURE"                                        },
  {0x4500, "SELECT OR RESELECT FAILURE"                                     },
  {0x4600, "UNSUCCESSFUL SOFT RESET"                                        },
  {0x4700, "SCSI PARITY ERROR"                                              },
  {0x4701, "DATA PHASE CRC ERROR DETECTED"                                  },
  {0x4702, "SCSI PARITY ERROR DETECTED DURING ST DATA PHASE"                },
  {0x4703, "INFORMATION UNIT iuCRC ERROR DETECTED"                          },
  {0x4704, "ASYNCHRONOUS INFORMATION PROTECTION ERROR DETECTED"             },
  {0x4705, "PROTOCOL SERVICE CRC ERROR"                                     },
  {0x477F, "SOME COMMANDS CLEARED BY ISCSI PROTOCOL EVENT"                  },
  {0x4800, "INITIATOR DETECTED ERROR MESSAGE RECEIVED"                      },
  {0x4900, "INVALID MESSAGE ERROR"                                          },
  {0x4A00, "COMMAND PHASE ERROR"                                            },
  {0x4B00, "DATA PHASE ERROR"                                               },
  {0x4B01, "INVALID TARGET PORT TRANSFER TAG RECEIVED"                      },
  {0x4B02, "TOO MUCH WRITE DATA"                                            },
  {0x4B03, "ACK NAK TIMEOUT"                                                },
  {0x4B04, "NAK RECEIVED"                                                   },
  {0x4B05, "DATA OFFSET ERROR"                                              },
  {0x4C00, "LOGICAL UNIT FAILED SELF-CONFIGURATION"                         },
  {0x4D00, "TAGGED OVERLAPPED COMMANDS (NN = TASK TAG)"                     },
  {0x4E00, "OVERLAPPED COMMANDS ATTEMPTED"                                  },
  {0x5300, "MEDIA LOAD OR EJECT FAILED"                                     },
  {0x5302, "MEDIUM REMOVAL PREVENTED"                                       },
  {0x5502, "INSUFFICIENT RESERVATION RESOURCES"                             },
  {0x5503, "INSUFFICIENT RESOURCES"                                         },
  {0x5504, "INSUFFICIENT REGISTRATION RESOURCES"                            },
  {0x5505, "INSUFFICIENT ACCESS CONTROL RESOURCES"                          },
  {0x5506, "AUXILIARY MEMORY OUT OF SPACE"                                  },
  {0x5A00, "OPERATOR REQUEST OR STATE CHANGE INPUT"                         },
  {0x5A01, "OPERATOR MEDIUM REMOVAL REQUEST"                                },
  {0x5B00, "LOG EXCEPTION"                                                  },
  {0x5B01, "THRESHOLD CONDITION MET"                                        },
  {0x5B02, "LOG COUNTER AT MAXIMUM"                                         },
  {0x5B03, "LOG LIST CODES EXHAUSTED"                                       },
  {0x5D00, "FAILURE PREDICTION THRESHOLD EXCEEDED"                          },
  {0x5DFF, "FAILURE PREDICTION THRESHOLD EXCEEDED (FALSE)"                  },
  {0x6500, "VOLTAGE FAULT"                                                  },
  {0x670A, "SET TARGET PORT GROUPS COMMAND FAILED"                          },

/* other device types? */
/*{0x0016, "5 XCOPY INTERFACE FAULT"                                        },*/
  {0x1300, "BARCODE UNREADABLE"                                             },
  {0x2606, "XCOPY INTERFACE FAULT 2606"                                     },
  {0x2609, "XCOPY INTERFACE FAULT 2609"                                     },
  {0x260A, "XCOPY INTERFACE FAULT 260A"                                     },
  {0x260B, "XCOPY INTERFACE FAULT 260B"                                     },
  {0x260C, "XCOPY INTERFACE FAULT 260C"                                     },
  {0x2E01, "XCOPY INTERFACE FAULT THIRD PARTY FAILURE"                      },
  {0x2E02, "XCOPY INTERFACE FAULT TARGET UNREACHABLE"                       },
  {0x2E03, "XCOPY INTERFACE FAULT 2E03"                                     },
  {0x2E04, "XCOPY INTERFACE FAULT TARGET DATA UNDERRUN"                     },
  {0x2E05, "XCOPY INTERFACE FAULT TARGET DATA OVERRUN"                      },

/* vendor unique */
  {0x0483, "LOGICAL UNIT NOT READY, DOOR OPEN"                   },
  {0x0483, "2 Door is open, Robot Disabled"                      },
  {0x0484, "2 Cover is off, Robot Disabled"                      },
  {0x4081, "4 Robotics Memory Failure"                           },
  {0x4082, "4 DRAM Memory Failure"                               },
  {0x40D1, "4 Entry Exit Door could not be extended"             },
  {0x40D2, "4 Entry Exit Door could not be retracted"            },
  {0x4C80, "4 Tape Scanning Failed"                              },
/*{0x8000, "4 Temperature Fault"                                 },*/
  {0x8101, "4 Move Failed, Drive Did Not Unload"                 },
  {0x8104, "4 Drive Failed to Come Ready"                        },
  {0x8404, "4 Duplicate SCSI ID"                                 },
  {0x8415, "4 Door Not Loaded"                                   },
  {0x8501, "4 Move Failed, Tape Left in Jaws"                    },
  {0x8502, "4 Move Failed, Tape Left in Source"                  },
  {0x8503, "4 Move Failed, Picker will reset"                    },
  {0x8504, "4 Long Axis Motor Blocked"                           },
  {0x8505, "4 Gripper Motor Blocked"                             },
  {0x8506, "4 Rotary Motor Blocked"                              },
  {0x8507, "4 Medium Axis Motor Blocked"                         },
  {0x8508, "4 Short Axis Motor Blocked"                          },
  {0x8509, "4 Parameter Block is Corrupted"                      },
  {0x850A, "4 Picker Failed To Park"                             },
  {0x850B, "4 Picker Failed To Initialize"                       },
  {0x850C, "4 Tape In Gripper"                                   },
  {0x850D, "4 Tape In Slot"                                      },
  {0x850E, "4 Tape Stuck In Slot"                                },
  {0x850F, "4 Tape Eject Timeout"                                },
  {0x8510, "4 No Tape In Gripper"                                },
  {0x8511, "4 Tape In Drive"                                     },
  {0x8512, "4 Drive Alignment Error"                             },
  {0x8513, "4 Tape Not Fully In Door"                            },
  {0x8514, "4 Tape In Door"                                      },
  {0x8516, "4 Drive Failed To Eject"                             },
  {0x8517, "4 Door Failed To Position"                           },
  {0x8518, "4 No Empty Slots In Carousel"                        },
  {0x8519, "4 No Loaded Slots In Carousel"                       },
  {0x851A, "4 Rotary Count Not In Range"                         },
  {0x851B, "4 Rotary Servo Oscillation"                          },
  {0x851C, "4 Robotic Picker Not Initialized"                    },
  {0x851D, "4 Rotary Jam"                                        },
  {0x851E, "4 Rotary Servo Not Regulated"                        },
  {0x851F, "4 Linear Servo Fault"                                },
  {0x8520, "4 Drive Eject Sensor Faulty"                         },
  {0x8521, "4 Slot Tab Blocking Drive"                           },
  {0x8522, "4 Linear Servo Fault2"                               },
  {0x8523, "4 Drive Not Installed"                               },
  {0x8524, "4 Linear Or Rotary Servo Fault"                      },
  {0x8525, "4 Tape Stuck In Drive"                               },
  {0x8526, "4 Scanner Not Available"                             },
  {0x8527, "4 Invalid Slot Number"                               },
  {0x8528, "4 Inventory Table Fault"                             },
  {0x8529, "4 Sweep Speed Out Of Range"                          },
  {0x852A, "4 Drive To Carousel Alignment Fault"                 },
  {0x852B, "4 Arm In Dead Zone"                                  },
  {0x852C, "4 Gripper Failed On Drive Fetch"                     },
  {0x852D, "4 Tape Pushed Back In Drive"                         },
  {0x852E, "4 Gripper Slipped Pulling Tape"                      },
  {0x852F, "4 Drive Faceplate Too Close"                         },
  {0x8530, "4 Bad Door Sensor Reading"                           },
  {0x8531, "4 Low Velocity Linear Servo Fault"                   },
  {0x8532, "4 Linear Servo Jam"                                  },
  {0x8533, "4 No Label On Tape"                                  },
  {0x8534, "4 Picker Not Initialized"                            },
  {0x8535, "4 Ambient Light Detected"                            },
  {0x8536, "4 Barcode Scanner Timeout"                           },
  {0x8537, "4 Rotary Crashstop Not Found"                        },
  {0x8538, "4 Rotary Index Pin Not Found"                        },
  {0x8539, "4 DSP Failed CRC Check"                              },
  {0x853A, "4 Linear Index Pin Not Found"                        },
  {0x853B, "4 Tape In Gripper2"                                  },
  {0x853C, "4 Guide Backstop Not Present"                        },
  {0x853D, "4 Reset Encoders Not Regulated"                      },
  {0x8590, "4 No Robotic Picker Version Defined"                 },
  {0x8591, "4 Calibration Block Not Found"                       },
  {0x8599, "4 General Robotics Failure"                          },
  {0x8600, "4 FQIP Failed Initialization"                        },
  {0x8602, "4 Excessive Drive Write Errors"                      },
  {0x8603, "4 Media Fault"                                       },
  {0x8604, "4 SCSI Parity Error"                                 },
  {0x8700, "4 Invalid FPROM, Invalid Id Bits"                    },
  {0x8701, "4 FPROM Erase Operation Failed"                      },
  {0x8702, "4 FPROM Write Operation Failed"                      },
  {0x8800, "4 General Picker Definition Error"                   },
  {0x8801, "4 Invalid Picker Type"                               },
  {0x8802, "4 Invalid Rack Type"                                 },
  {0x8803, "4 Invalid Library Size"                              },
  {0x8804, "4 Invalid Chassis Type"                              },
  {0x8805, "4 Invalid EE Door Type"                              },
  {0x9000, "4 CAN Command Timeout"                               },
  {0x9001, "4 Incomplete Configuration Received"                 },
  {0x9002, "4 FQIP Fibre Interface Fault"                        },
  {0x9003, "4 FQIP Fibre Interface Fault2"                       },
  {0x9004, "4 FQIP Fibre Interface Fault3"                       },
  {0x9200, "4 FQIP SCSI Interface Fault00"                       },
  {0x9201, "4 FQIP SCSI Interface Fault01"                       },
  {0x9202, "4 FQIP SCSI Interface Fault02"                       },
  {0x9203, "4 FQIP SCSI Interface Fault03"                       },
  {0x9204, "4 FQIP SCSI Interface Fault04"                       },
  {0x9205, "4 FQIP SCSI Interface Fault05"                       },
  {0x9206, "4 FQIP SCSI Interface Fault06"                       },
  {0x9207, "4 FQIP SCSI Interface Fault07"                       },
  {0x9208, "4 FQIP SCSI Interface Fault08"                       },
  {0x9209, "4 FQIP SCSI Interface Fault09"                       },
  {0x920A, "4 FQIP SCSI Interface Fault0A"                       },
  {0x920B, "4 FQIP SCSI Interface Fault0B"                       },
  {0x920C, "4 FQIP SCSI Interface Fault0C"                       },
  {0x920D, "4 FQIP SCSI Interface Fault0D"                       },
  {0x920E, "4 FQIP SCSI Interface Fault0E"                       },
  {0x920F, "4 FQIP SCSI Interface Fault0F"                       },
  {0x9210, "4 FQIP SCSI Interface Fault10"                       },
  {0x9211, "4 FQIP SCSI Interface Fault11"                       },
  {0x9212, "4 FQIP SCSI Interface Fault12"                       },
  {0x9213, "4 FQIP SCSI Interface Fault13"                       },
  {0x9214, "4 FQIP SCSI Interface Fault14"                       },
  {0x9215, "4 FQIP SCSI Interface Fault15"                       },
  {0x9216, "4 FQIP SCSI Interface Fault16"                       },
  {0x9217, "4 FQIP SCSI Interface Fault17"                       },
  {0x9218, "4 FQIP SCSI Interface Fault18"                       },
  {0x9219, "4 FQIP SCSI Interface Fault19"                       },
  {0x921A, "4 FQIP SCSI Interface Fault1A"                       },
  {0x921B, "4 FQIP SCSI Interface Fault1B"                       },
  {0x921C, "4 FQIP SCSI Interface Fault1C"                       },
  {0x921D, "4 FQIP SCSI Interface Fault1D"                       },
  {0x921E, "4 FQIP SCSI Interface Fault1E"                       },
  {0x921F, "4 FQIP SCSI Interface Fault1F"                       },
  {0x9220, "4 FQIP SCSI Interface Fault20"                       },
  {0x9221, "4 FQIP SCSI Interface Fault21"                       },
  {0x9222, "4 FQIP SCSI Interface Fault22"                       },
  {0x9223, "4 FQIP SCSI Interface Fault23"                       },
  {0x9224, "4 FQIP SCSI Interface Fault24"                       },
  {0x9225, "4 FQIP SCSI Interface Fault25"                       },
  {0x9226, "4 FQIP SCSI Interface Fault26"                       },
  {0x9227, "4 FQIP SCSI Interface Fault27"                       },
  {0x9228, "4 FQIP SCSI Interface Fault28"                       },
  {0x9229, "4 FQIP SCSI Interface Fault29"                       },
  {0x922A, "4 FQIP SCSI Interface Fault2A"                       },
  {0x922B, "4 FQIP SCSI Interface Fault2B"                       },
  {0x922C, "4 FQIP SCSI Interface Fault2C"                       },
  {0x922D, "4 FQIP SCSI Interface Fault2D"                       },
  {0x922E, "4 FQIP SCSI Interface Fault2E"                       },
  {0x922F, "4 FQIP SCSI Interface Fault2F"                       },
  {0x9901, "4 Invalid CAN Command During Code Load"              },
  {0x9902, "4 Invalid CAN Address During Code Load"              },
  {0x9903, "4 Invalid CAN From Address During Code Load"         },
  {0x9904, "4 Invalid CAN Sequence During Code Load"             },
  {0x9905, "4 Checksum Failed During Code Load"                  },
  {0x9906, "4 Repeated Sequence During Code Load"                },
  {0x9907, "4 Frame Error During Code Load"                      },
  {0x9908, "4 Burn Code to Flash Failed"                         },
  {0x9909, "4 Invalid CAN XID During Code Load"                  },
  {0x990A, "4 Invalid CAN Packet Length During Code Load"        },
  {0x990B, "4 End of Packet Error During Code Load"              },
  {0x990C, "4 PostProcessing Failed During Code Load"            },

  {0x3D80, "5 SCSI Disconnects Must Be Allowed"                  },
  {0x8000, "5 Generic Invalid Move"                              },
  {0x8001, "5 Picker Not Empty"                                  },
  {0x8003, "5 Source Magazine Not Available"                     },
  {0x8004, "5 Destination Magazine Not Available"                },
  {0x8005, "5 Source Drive Not Available"                        },
  {0x8006, "5 Destination Drive Not Available"                   },
  {0x8007, "5 Medium Source Element Invalid No Barcode Label"    },
  {0x8008, "5 Magazine Has Duplicate Barcode Label"              },
  {0x8018, "5 Conflict, Element is Reserved"                     },
  {0x8019, "5 Magazine Media Mismatch"                           },
  {0x8100, "5 Duplicate SCSI ID on this Bus"                     },
  {0x8101, "5 Entry Exit Door is Exit Only"                      },
  {0x8102, "5 Library Full"                                      },
  {0x8103, "5 Gripper Arm Not Empty"                             },
  {0x8110, "5 Unable To Insert EE Is Exit Only"                  },
  {0x8603, "5 Drive Needs Cleaning"                              },

  {0x2980, "6 Drive Failed Post"                                 },
  {0x8601, "6 Excessive Drive Write Errors"                      },
  {0x8603, "6 Media Warning"                                     },
  {0x8604, "6 Excessive Drive Read Errors"                       },
  {0x8605, "6 Tape Exceeded Load Limit"                          },

  {0x8300, "9 Barcode Label is Unread"                           },
  {0x8301, "9 Problem Reading Barcode"                           },
  {0x8311, "9 Unmounted Tape In Drive"                           },
  {0x8312, "9 Tape Unloaded In Drive Door"                       },
  {0x8313, "9 Two Tapes In Drive Guide"                          },
  {0x8302, "9 Tape is Reserved for Front Panel"                  },
  {0x8400, "9 Unsupported SCSI Comman"                           },
  {0x8401, "9 No response from SCSI Target"                      },
  {0x8402, "9 Check Condition from Target"                       },
  {0x8403, "9 SCSI ID Same As Library ID"                        },
  {0x8404, "9 Emulation Error Drives 1 And 2"                    },
  {0x8405, "9 Emulation Error Drives 3 And 4"                    },
  {0x8406, "9 Bus Mode Error"                                    },
  {0x8408, "9 Target Device Busy"                                },
  {0x8418, "9 SCSI Reservation Conflict"                         },

/* Vendor Unique -- Python   */

  {0x8900, "2 DRIVE NOT POWERED ON"                              },
  {0x8906, "2 MOTOR BRIDGE NOT ENABLED"                          },
  {0x8909, "2 INVALID ACCEL DECEL"                               },

  {0x8901, "4 DRIVE NOT RESPONDING"                              },
  {0x8902, "4 BARCODE CMD TIMEOUT"                               },
  {0x8903, "4 MOTOR HW OVER CURRENT"                             },
  {0x8904, "4 MOTOR OVER VELOCITY"                               },
  {0x8905, "4 MOTOR STALL"                                       },
  {0x8907, "4 MOTOR MOVE IN PROGRESS"                            },
  {0x8908, "4 UNKNOWN DRIVE TYPE"                                },
  {0x8917, "4 MAGAZINE STATE CHANGE"                             },

  {0x890A, "4 BARCODE UNKNOWN FRAME"                             },
  {0x890B, "4 BARCODE UNKNOWN GROUP"                             },
  {0x890C, "4 BARCODE UNKNOWN ID"                                },
  {0x890D, "4 BARCODE INVALID PARM"                              },
  {0x890E, "4 BARCODE UNKNOWN ERROR"                             },
  {0x890F, "4 BARCODE MULTIFRAME ERROR"                          },
  {0x891A, "4 BARCODE FRAME NUM ERROR"                           },
  {0x891B, "4 BARCODE NOT IMPLEMENTED"                           },
  {0x891C, "4 BARCODE BAD CHKSUM"                                },

  /* Motion hard failures (dead and stays dead) */
  {0x8910, "4 VERTICAL AXIS BLOCKED"                             },
  {0x8911, "4 HORIZONTAL AXIS BLOCKED"                           },
  {0x8912, "4 SIDE AXIS BLOCKED"                                 },
  {0x8913, "4 MAGAZINE AXIS BLOCKED"                             },
  {0x8914, "4 TOGGLE AXIS BLOCKED"                               },
  {0x8915, "4 PICK AXIS BLOCKED"                                 },
  {0x8916, "4 ROTARY AXIS BLOCKED"                               },

  /* Motion soft failures */
  {0x8920, "5 LIBRARY FULL"                                      },

/* Vendor Unique -- Doom RXT */

  {0x8080, "9 Programmer Error"                                  },
  {0xA003, "4 FIRMWARE FAILURE BASE DRIVES"                      },
  {0xA004, "4 FIRMWARE FAILURE OS CALL"                          },
  {0xA005, "4 FIRMWARE FAILURE APP START"                        },
  {0xA001, "4 FIRMWARE FAILURE GPIO INIT"                        },
  {0xA002, "4 FIRMWARE FAILURE CORE"                             },

  {-1, NULL}
};


void
PrintRequestSenseSub(VECTOR dat)
{
  unsigned int code, seg, keyf, info, len, csinfo, asc, ascq, fru, sksv, fieldp;
  unsigned int key = 0x10;
  char *keyascq = NULL;
  const char *ascqname = NULL;

  printf("Request Sense:\n");
  code   = dat.dat[ 0];
  seg    = dat.dat[ 1];
  keyf   = dat.dat[ 2];
  info   =
    (dat.dat[ 3] << 24) |
    (dat.dat[ 4] << 16) |
    (dat.dat[ 5] <<  8) |
    (dat.dat[ 6] <<  0) |
    0;
  len    = dat.dat[ 7];
  csinfo =
    (dat.dat[ 8] << 24) |
    (dat.dat[ 9] << 16) |
    (dat.dat[10] <<  8) |
    (dat.dat[11] <<  0) |
    0;
  asc    = dat.dat[12];
  ascq   = dat.dat[13];
  fru    = dat.dat[14];
  sksv   = dat.dat[15];
  fieldp =
    (dat.dat[16] <<  8) |
    (dat.dat[17] <<  0) |
    0;

  if (dat.len == 0) {
    printf("  no data\n");
    return;
  }
  if((signed)len > dat.len - 8)
    len = dat.len - 8;
  if (dat.len>=3) {
    key = keyf & 0xf;
    keyascq = strdup(keyname[key]);
  }
  if (dat.len>=2) {
    printf("  code: %2x, seg: %d\n", code&0x7f, seg);
  } else {
    printf("  code: %2x\n"         , code&0x7f);
  }
  if (len>=6) {
    ascqname = lookup(asc_q, (asc<<8) | ascq);
    if (asc == 0x40 && ascq >= 0x80) {
      ascqname = lookup(asc_q, (asc<<8) | 0xff);
    }
    if (ascqname != NULL) {
      char *save_key = keyascq;
      keyascq = (char*)malloc(strlen(save_key) + 2 + strlen(ascqname) + 1);
      strcpy(keyascq, save_key);
      strcat(keyascq, ": ");
      strcat(keyascq, ascqname);
      free(save_key);
    }
    printf("  key/ASC/Q: %x/%.2x/%.2x (%s)\n", key, asc, ascq, keyascq);
  } else if (len>=5) {
    printf("  key/ASC: %x/%.2x (%s)\n"       , key, asc,       keyascq);
  } else if (dat.len>=3) {
    printf("  key: %x (%s)\n"                , key,            keyascq);
  }
  free(keyascq);
  if (dat.len>=3)
    printf("  FM=%s, EOM=%s, ILI=%s\n", (keyf&0x80)?"T":"F", (keyf&0x40)?"T":"F", (keyf&0x20)?"T":"F");
  if (code&0x80 && dat.len>=7)
    printf("  info:      0x%.8x (%d)\n",   info,   info);
  if (len>=4)
    printf("  more info: 0x%.8x (%d)\n", csinfo, csinfo);
  if (len>=7)
    printf("  FRU: 0x%.2x (%d)\n", fru, fru);
  if (len>=8 && sksv&0x80) {
    printf("  SKSV:\n");
    if (key == 5) {
      /* Illegal Request */
      printf("    %s ", sksv&0x40 ? "Command" : "Data");
      if (sksv&0x08)
        printf("bit %d ", sksv&0x7);
      printf("byte %d (0x%.2x)\n", fieldp, fieldp);
    }
    if (key == 1 || key == 3 || key == 4) {
      /* Recovered Error, Medium Error, Hardware Error */
      printf("    Actual Retry Count: %d\n", fieldp);
    }
    if (key == 2) {
      /* Not Ready */
      printf("    Progress Indication: %d\n", fieldp);
    }
  }
  if (len>10) {
    unsigned int num, byte;
    printf("  more bytes:\n");
    printf("    #0012:      ");
    for (num=18; num<len+8; num++) {
      byte = dat.dat[num];
      if (num%8 == 0) {
        printf("    #%.4x:", num);
      }
      printf(" %.2x", byte);
      if ((num+1)%8 == 0) {
        printf("\n");
      }
    }
    if (num%8 != 0) {
      printf("\n");
    }
  }
}


