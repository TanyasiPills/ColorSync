interface PopoverProps {
  isOpen: boolean;
  onClose: () => void;
}

export function Posting({ isOpen, onClose }: PopoverProps) {
  if (!isOpen) return null;

  return (
    <div className="posting">
      <h2>Popover</h2>
      <hr />
      <p>A popover is an element that is placed on top of everything else.</p>
      <p>It can be used when you want to tell something important.</p>
      <button onClick={onClose}>Close</button>
    </div>
  );
}
