import 'package:flutter/material.dart';

class TimeSyncCard extends StatelessWidget {
  const TimeSyncCard({
    super.key,
    required this.enabled,
    required this.onPressed,
  });

  final bool enabled;
  final VoidCallback onPressed;

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(18),
      decoration: BoxDecoration(
        gradient: LinearGradient(
          colors: enabled
              ? [const Color(0xFF7768D8), const Color(0xFF9C70D5)]
              : [const Color(0xFFD4D0DF), const Color(0xFFC5C1CF)],
        ),
        borderRadius: BorderRadius.circular(24),
        boxShadow: enabled
            ? [
                BoxShadow(
                  color: const Color(0xFF7768D8).withValues(alpha: 0.22),
                  blurRadius: 22,
                  offset: const Offset(0, 10),
                ),
              ]
            : null,
      ),
      child: Row(
        children: [
          const Icon(Icons.schedule_rounded, color: Colors.white, size: 30),
          const SizedBox(width: 13),
          const Expanded(
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text(
                  'Sync clock',
                  style: TextStyle(
                    color: Colors.white,
                    fontWeight: FontWeight.w800,
                    fontSize: 16,
                  ),
                ),
                SizedBox(height: 2),
                Text(
                  'Send current device time',
                  style: TextStyle(color: Color(0xFFEDE9FF)),
                ),
              ],
            ),
          ),
          IconButton(
            onPressed: enabled ? onPressed : null,
            style: IconButton.styleFrom(
              backgroundColor: Colors.white.withValues(alpha: 0.18),
              foregroundColor: Colors.white,
              disabledForegroundColor: Colors.white54,
            ),
            tooltip: 'Sync device time',
            icon: const Icon(Icons.sync_rounded),
          ),
        ],
      ),
    );
  }
}
